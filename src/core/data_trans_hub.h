#ifndef DATATRANSHUB_DATA_TRANS_HUB_H
#define DATATRANSHUB_DATA_TRANS_HUB_H

#include <functional>
#include <string>
#include <atomic>
#include <list>
#include <map>
#include "thread_manager.h"
#include "period_task_manager.h"
#include "mmaped_file.h"
#include "buffer.h"
#include "log_buffer.h"
#include "thread_base_timer.h"

namespace future {
    class DataTransHub {
    public:
        enum class UploadTriggerWay {
            Manual,
            Auto,
        };


    public:

        static DataTransHub *
        MakeDataTransHub(const std::string &cacheDir, const std::string &dataDir,
                         const std::string &encryptKey);

        static void Destroy(DataTransHub *dataTransHub);

        void DestroyThis();

        void SetUploadImp(std::function<void(
                const std::string &filePath)> uploadImp);

        void SetFileMaxSize(std::size_t fileMaxSize);

        void SetBufferSize(std::size_t bufferSize);

        //unit s
        void SetExpiredTime(std::int64_t expiredTime);

        void SetReportingInterval(std::int64_t reportingInterval);

        void SetRetryInterval(std::int64_t retryInterval);

        void SetDataFilePrefix(const std::string &prefix);

        void SetUploadTriggerWayWay(UploadTriggerWay uploadTriggerWay);

        void SetWriteDiskPeriod(std::size_t period);

        void Start();

        void ReaWaken();

        void ManualTriggerUpload(std::function<void(void)> finish);

        void Push(const std::vector<unsigned char> &data);

        void NotifyUploadSuccess(std::string filePath);

        void NotifyUploadFailed(std::string filePath);


    private:
        DataTransHub(const std::string &cacheDir, const std::string &dataDir,
                     const std::string &encryptKey);

        ~DataTransHub();

        void Open();

        void Close();

        std::size_t GetIndexPos(const std::string &fileName);

        std::size_t GetIndex(const std::string &fileName);

        std::string MakeDataFile(const std::string &dir, const std::string &prefix,
                                 const std::string &lastFilePath);

        std::shared_ptr<std::list<std::string> > GetOrderedDataFiles(const std::string &dir);

        std::string GetLastDataFile(const std::string &dir, const std::string &prefix);

        std::string
        GetWriteFile(const std::string &dir, const std::string &prefix, std::size_t writeSize);

        bool IsExpired(std::int64_t date, std::int64_t expiredTime);

        void DeleteExpiredFile(const std::string &dir);

        void WriteToFile(AutoBuffer &buffer);

        void WriteDiskTask();

        void Restore();

        bool IsRectifyFileName(const std::string &filePath);

        std::string RectifyFileName(const std::string &filePath, std::size_t index);

        void Upload();

    private:

        std::string m_CacheDir;
        std::string m_DataDir;
        std::string m_EncryptKey;
        std::atomic_uint m_MaxFileSize;
        std::atomic_int_least64_t m_ExpiredTime;
        std::atomic_int_least64_t m_ReportingInterval;
        std::string m_Prefix;
        std::shared_ptr<MmapedFile> m_MmapedFileCache;
        std::shared_ptr<Buffer> m_Buffer;
        std::shared_ptr<LogBuffer> m_LogBuffer;
        ThreadManager *m_ThreadManager;
        PeriodTaskManager *m_PeriodTaskManager;
        std::atomic_uint m_BufferSize;
        std::recursive_mutex m_Mut;
        std::mutex m_WriteFileMut;
        std::shared_ptr<Timer> m_UploadDelayTasks;
        std::shared_ptr<Timer> m_WriteDiskTasks;

        std::function<void(std::string &filePath)> m_UploadImp;
        std::size_t m_Step;
        std::atomic_uint m_NextUploadTime;
        UploadTriggerWay m_UploadTriggerWay;
        std::atomic_uint m_WriteTaskPeriod;
        std::map<std::string, std::function<void(void)>> m_FileToCallback;
        bool m_IsStart;
    };
}

#endif //DATATRANSHUB_DATA_TRANS_HUB_H
