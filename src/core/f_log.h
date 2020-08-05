//
// Created by bingjian on 2020/4/12.
//

#ifndef DATATRANSHUB_F_LOG_H
#define DATATRANSHUB_F_LOG_H

#include <string>
#include "data_trans_hub.h"

namespace future {

    class F_Log {

    public:
        enum class Priority {
            VERBOSE = 0,
            DEBUG,
            INFO,
            WARNING,
            ERROR,
            FATAL,
            NONE
        };

        struct Info {
            Priority level;
            std::string tag;
            std::string filename;
            std::string funcname;
            std::size_t line;
            std::int64_t pid;
            std::int64_t tid;
            std::int64_t maintid;
        };

    public:
        static F_Log *MakeLog(const std::string &cacheDir, const std::string &dataDir,
                              const std::string &encryptKey);

        static void Destroy(F_Log *log);

        F_Log(const std::string &cacheDir, const std::string &dataDir,
              const std::string &encryptKey);

        ~F_Log();

        void SetUploadImp(std::function<void(
                const std::string &filePath)> uploadImp);

        void SetFileMaxSize(std::size_t fileMaxSize);

        void SetBufferSize(std::size_t bufferSize);

        //unit s
        void SetExpiredTime(std::int64_t expiredTime);

        void SetReportingInterval(std::int64_t reportingInterval);

        void SetRetryInterval(std::int64_t retryInterval);

        void SetDataFilePrefix(const std::string &prefix);

        void SetUploadTriggerWayWay(DataTransHub::UploadTriggerWay uploadTriggerWay);

        void SetWriteDiskPeriod(std::size_t period);

        void Start();

        void ReaWaken();

        void ManualTriggerUpload(std::function<void(void)> finish);

        void Write(Info &info, const std::vector<unsigned char> &data);
        
        void WriteData(const std::vector<unsigned char> &data);

        void NotifyUploadSuccess(std::string filePath);

        void NotifyUploadFailed(std::string filePath);

    private:
        DataTransHub *dataTransHub_;
    };
}

#endif //DATATRANSHUB_F_LOG_H
