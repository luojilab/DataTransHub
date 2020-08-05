//
// Created by bingjian on 2019/11/21.
//

#ifndef NET_DISPATCHER_SAFE_IO_H
#define NET_DISPATCHER_SAFE_IO_H

#include <stdlib.h>
#include <string>

namespace future {
    class SafeIO {
    public:
        SafeIO(const std::string &rea_cache_tag,
               const std::string &write_cache_tag,
               const std::string &append_tag);
        
        ~SafeIO();

        FILE *OpenForRead(const std::string &path, const std::string &mode);

        FILE *OpenForWrite(const std::string &path, const std::string &mode);

        FILE *OpenForAppend(const std::string &path, const std::string &mode);

        int Seek(FILE *fp, int offset, int whence);

        size_t Read(void* buf ,size_t count, FILE *fp);

        size_t Write(void *buf, size_t count, FILE *fp);

        int CloseForRead(FILE *fp);

        int CloseForWrite(FILE *fp);

        int CloseForAppend(FILE *fp);

    private:
        std::string read_cache_tag_;
        std::string write_cache_tag_;
        std::string write_path_;
        std::string original_path_;
        std::string append_tag_;
        std::string append_path_;

    };
}
#endif //NETDISPATCHER_SAFE_IO_H
