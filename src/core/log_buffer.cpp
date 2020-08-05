#include <cstdio>
#include <time.h>
#include <algorithm>
#include <sys/time.h>
#include <string.h>
#include <errno.h>
#include <assert.h>

#include "log_buffer.h"
#include "crypto.h"


#ifdef WIN32
#define snprintf _snprintf
#endif

namespace future {
    bool LogBuffer::GetPeriodLogs(const char *_log_path, int _begin_hour, int _end_hour,
                                  unsigned long &_begin_pos, unsigned long &_end_pos,
                                  std::string &_err_msg) {
        return LogCrypt::GetPeriodLogs(_log_path, _begin_hour, _end_hour, _begin_pos, _end_pos,
                                       _err_msg);
    }

    LogBuffer::LogBuffer(void *_pbuffer, size_t _len, bool _isCompress, const char *_pubkey)
            : is_compress_(_isCompress), log_crypt_(new LogCrypt(_pubkey)), remain_nocrypt_len_(0) {
        buff_.Attach(_pbuffer, _len);
        __Fix();

        if (is_compress_) {
            memset(&cstream_, 0, sizeof(cstream_));
        }
    }

    LogBuffer::~LogBuffer() {
        if (is_compress_ && Z_NULL != cstream_.state) {
            deflateEnd(&cstream_);
        }

        delete log_crypt_;
    }

    PtrBuffer &LogBuffer::GetData() {
        return buff_;
    }


    void LogBuffer::Flush(AutoBuffer &_buff) {

        if (is_compress_ && Z_NULL != cstream_.state) {
            deflateEnd(&cstream_);
        }

        if (log_crypt_->GetLogLen((char *) buff_.Ptr(), buff_.Length()) == 0) {
            __Clear();
            return;
        }

        __Flush();
        _buff.Write(buff_.Ptr(), buff_.Length());
        __Clear();
    }

    bool LogBuffer::Write(const void *_data, size_t _inputlen, AutoBuffer &_out_buff) {
        if (NULL == _data || 0 == _inputlen) {
            return false;
        }

        log_crypt_->CryptSyncLog((char *) _data, _inputlen, _out_buff);

        return true;
    }


    bool LogBuffer::Write(const void *_data, size_t _length) {
        if (NULL == _data || 0 == _length) {
            return false;
        }

        if (buff_.Length() == 0) {
            if (!__Reset()) return false;
        }

        size_t before_len = buff_.Length();
        size_t write_len = _length;

        if (is_compress_) {
            cstream_.avail_in = (uInt) _length;
            cstream_.next_in = (Bytef *) _data;

            uInt avail_out = (uInt) (buff_.MaxLength() - log_crypt_->GetTailerLen() - buff_.Length());
            cstream_.next_out = (Bytef *) buff_.PosPtr();
            cstream_.avail_out = avail_out;

            if (Z_OK != deflate(&cstream_, Z_SYNC_FLUSH)) {
                return false;
            }

            write_len = avail_out - cstream_.avail_out;
        } else {
            buff_.Write(_data, _length);
        }

        before_len -= remain_nocrypt_len_;

        AutoBuffer out_buffer;
        size_t last_remain_len = remain_nocrypt_len_;
        log_crypt_->CryptAsyncLog((char *) buff_.Ptr() + before_len,
                                  write_len + remain_nocrypt_len_, out_buffer, remain_nocrypt_len_);

        buff_.Write(out_buffer.Ptr(), out_buffer.Length(), before_len);

        before_len += out_buffer.Length();
        buff_.UpdateLen(before_len, before_len);

        log_crypt_->UpdateLogLen((char *) buff_.Ptr(),
                                 (uint32_t) (out_buffer.Length() - last_remain_len));

        return true;
    }

    bool LogBuffer::__Reset() {

        __Clear();

        if (is_compress_) {
            cstream_.zalloc = Z_NULL;
            cstream_.zfree = Z_NULL;
            cstream_.opaque = Z_NULL;

            if (Z_OK !=
                deflateInit2(&cstream_, Z_BEST_COMPRESSION, Z_DEFLATED, -MAX_WBITS, MAX_MEM_LEVEL,
                             Z_DEFAULT_STRATEGY)) {
                return false;
            }

        }

        log_crypt_->SetHeaderInfo((char *) buff_.Ptr(), is_compress_);
        buff_.UpdateLen(log_crypt_->GetHeaderLen(), log_crypt_->GetHeaderLen());

        return true;
    }

    void LogBuffer::__Flush() {
        assert(buff_.Length() >= log_crypt_->GetHeaderLen());

        log_crypt_->UpdateLogTime((char *) buff_.Ptr());
        log_crypt_->SetTailerInfo((char *) buff_.Ptr() + buff_.Length());
        buff_.UpdateLen(buff_.Length() + log_crypt_->GetTailerLen(),
                        buff_.Length() + log_crypt_->GetTailerLen());

    }

    void LogBuffer::__Clear() {
        memset(buff_.Ptr(), 0, buff_.Length());
        buff_.UpdateLen(0, 0);
        remain_nocrypt_len_ = 0;
    }


    void LogBuffer::__Fix() {
        uint32_t raw_log_len = 0;
        bool is_compress = false;
        if (log_crypt_->Fix((char *) buff_.Ptr(), buff_.Length(), is_compress, raw_log_len)) {
            buff_.UpdateLen(raw_log_len + log_crypt_->GetHeaderLen(),
                            raw_log_len + log_crypt_->GetHeaderLen());
        } else {
            buff_.UpdateLen(0, 0);
        }

    }
}
