//
// Created by bingjian on 2020/4/12.
//

#include "f_log.h"
#include "time_util.h"
#include "string_util.h"


namespace future {

    F_Log *F_Log::MakeLog(const std::string &cacheDir, const std::string &dataDir,
                          const std::string &encryptKey) {
        return new F_Log(cacheDir, dataDir, encryptKey);
    }

    void F_Log::Destroy(F_Log *log) {
        if (log) {
            delete log;
        }
    }

    F_Log::F_Log(const std::string &cacheDir, const std::string &dataDir,
                 const std::string &encryptKey) {
        dataTransHub_ = DataTransHub::MakeDataTransHub(cacheDir, dataDir, encryptKey);
    }

    F_Log::~F_Log() {
        DataTransHub::Destroy(dataTransHub_);
    }

    void F_Log::SetUploadImp(std::function<void(
            const std::string &filePath)> uploadImp) {
        dataTransHub_->SetUploadImp(uploadImp);
    }

    void F_Log::SetFileMaxSize(std::size_t fileMaxSize) {
        dataTransHub_->SetFileMaxSize(fileMaxSize);
    }

    void F_Log::SetBufferSize(std::size_t bufferSize) {
        dataTransHub_->SetBufferSize(bufferSize);
    }

    //unit s
    void F_Log::SetExpiredTime(std::int64_t expiredTime) {
        dataTransHub_->SetExpiredTime(expiredTime);
    }

    void F_Log::SetReportingInterval(std::int64_t reportingInterval) {
        dataTransHub_->SetReportingInterval(reportingInterval);
    }

    void F_Log::SetRetryInterval(std::int64_t retryInterval) {
        dataTransHub_->SetRetryInterval(retryInterval);
    }

    void F_Log::SetDataFilePrefix(const std::string &prefix) {
        dataTransHub_->SetDataFilePrefix(prefix);
    }


    void F_Log::SetUploadTriggerWayWay(DataTransHub::UploadTriggerWay uploadTriggerWay) {
        dataTransHub_->SetUploadTriggerWayWay(uploadTriggerWay);
    }

    void F_Log::SetWriteDiskPeriod(std::size_t period) {
        dataTransHub_->SetWriteDiskPeriod(period);
    }

    void F_Log::Start() {
        dataTransHub_->Start();
    }

    void F_Log::ReaWaken() {
        dataTransHub_->ReaWaken();
    }

    void F_Log::ManualTriggerUpload(std::function<void(void)> finish) {
        dataTransHub_->ManualTriggerUpload(finish);
    }

    void F_Log::Write(Info &info, const std::vector<unsigned char> &data) {
        std::string log;
        std::string timeStr = TimeUtil::GetFormatDate("%Y-%m-%d %H:%M:%S.%MS");
        log.append(timeStr);
        log.append(" pid:");
        log.append(Int64ToStr(info.pid));
        log.append(" tid:");
        log.append(Int64ToStr(info.tid));
        log.append(" maintid:");
        log.append(Int64ToStr(info.maintid));
        log.append(" level:");
        switch (info.level) {
            case Priority::VERBOSE:
                log.append("V");
                break;
            case Priority::DEBUG:
                log.append("D");
                break;
            case Priority::INFO:
                log.append("I");
                break;
            case Priority::WARNING:
                log.append("W");
                break;
            case Priority::ERROR:
                log.append("E");
                break;
            case Priority::FATAL:
                log.append("F");
                break;
            case Priority::NONE:
                log.append("null");
                break;
        }
        log.append(" tag:");
        log.append(info.tag);
        log.append(" filename:");
        if (info.filename.empty()) {
            log.append("null");
        } else {
            log.append(info.filename);
        }
        log.append(" funname:");
        if (info.funcname.empty()) {
            log.append("null");
        } else {
            log.append(info.funcname);
        }
        log.append(" line:");
        log.append(Int64ToStr(info.line));
        log.append(" c:");
        log.insert(log.cend(), data.begin(), data.end());
        StringReplace(log, "\n", " ");
        std::vector<unsigned char> formatedData;
        formatedData.assign(log.begin(), log.end());
        WriteData(formatedData);
    }

    void F_Log::WriteData(const std::vector<unsigned char> &data) {
        dataTransHub_->Push(data);
    }

    void F_Log::NotifyUploadSuccess(std::string filePath) {
        dataTransHub_->NotifyUploadSuccess(filePath);
    }

    void F_Log::NotifyUploadFailed(std::string filePath) {
        dataTransHub_->NotifyUploadFailed(filePath);
    }
}
