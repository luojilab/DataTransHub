#include "data_trans_hub.h"
#include "file.h"
#include "time_util.h"
#include "string_util.h"
#include "safe_io.h"
#include "log.h"


namespace future {
    const std::string DATA_SUFFIX = ".data";
    const std::string CACHE_SUFFIX = ".mmap";
    const std::size_t MAX_WAIT_TIME = 1000 * 60 * 60;
    const std::size_t MAX_INDEX = 100000;

    DataTransHub *
    DataTransHub::MakeDataTransHub(const std::string &cacheDir, const std::string &dataDir,
                                   const std::string &encryptKey) {
        if (dataDir.empty()) {
            return nullptr;
        }
        return new DataTransHub(cacheDir, dataDir, encryptKey);
    }

    void DataTransHub::Destroy(DataTransHub *dataTransHub) {
        dataTransHub->DestroyThis();
    }

    DataTransHub::DataTransHub(const std::string &cacheDir,
                               const std::string &dataDir,
                               const std::string &encryptKey)
            : m_CacheDir(cacheDir), m_DataDir(dataDir), m_EncryptKey(encryptKey),
              m_MaxFileSize(150 * 1024),
              m_Prefix("dth"), m_ExpiredTime(0),
              m_ReportingInterval(1000 * 5),
              m_BufferSize(75 * 1024),
              m_Step(5 * 1000),
              m_NextUploadTime(0),
              m_WriteTaskPeriod(0),
              m_UploadTriggerWay(UploadTriggerWay::Auto),
              m_UploadImp(nullptr),
              m_IsStart(false) {
        if (cacheDir.empty()) {
            m_CacheDir = dataDir;
        }
        m_ThreadManager = ThreadManager::MakeThreadManager();
        m_PeriodTaskManager = PeriodTaskManager::MakePeriodTaskManager(m_ThreadManager);
    }

    void DataTransHub::DestroyThis() {
        m_ThreadManager->CallOnMainThread([this]() {
            delete this;
        });
    }

    DataTransHub::~DataTransHub() {
        Close();
        ThreadManager::DestroyThreadManager(m_ThreadManager);
        PeriodTaskManager::DestroyPeriodTaskManager(m_PeriodTaskManager);
    }

    void DataTransHub::SetUploadImp(std::function<void(
            const std::string &filePath)> uploadImp) {
        if (!m_ThreadManager->IsMainThread()) {
            m_ThreadManager->CallOnMainThread([this, uploadImp]() {
                SetUploadImp(uploadImp);
            });
            return;
        }

        m_UploadImp = uploadImp;
    }

    void DataTransHub::Open() {
        if (!File::IsFileExist(m_CacheDir)) {
            File::MkPath(m_CacheDir);
        }
        if (!File::IsFileExist(m_DataDir)) {
            File::MkPath(m_DataDir);
        }
        std::string cacheFilePath;
        cacheFilePath.append(m_CacheDir).append("/").append(m_Prefix).append(
                "_data_trans_hub_cache").append(CACHE_SUFFIX);
        m_MmapedFileCache = std::make_shared<MmapedFile>(cacheFilePath, m_BufferSize);
        if (m_MmapedFileCache->Open()) {
            m_Buffer = std::make_shared<Buffer>(m_MmapedFileCache->GetMemBegin(),
                                                m_MmapedFileCache->GetMaxSize(),
                                                BufferCopyFlag::BufferNoCopy);
        } else {
            m_Buffer = std::make_shared<Buffer>(m_BufferSize);
        }

        m_LogBuffer = std::make_shared<LogBuffer>(m_Buffer->GetBegin(), m_Buffer->Length(), true,
                                                  m_EncryptKey.c_str());
    }

    void DataTransHub::Close() {
        if (m_MmapedFileCache->IsOpened()) {
            WriteDiskTask();
            m_MmapedFileCache->Close();
        }
    }

    std::size_t DataTransHub::GetIndexPos(const std::string &fileName) {
        return fileName.rfind("_");
    }

    std::size_t DataTransHub::GetIndex(const std::string &fileName) {
        std::size_t pos = GetIndexPos(fileName);
        if (pos != std::string::npos && pos + 1 < fileName.size()) {
            std::string indexStr = fileName.substr(pos + 1);
            return StrToInt64(indexStr);
        }
        return std::string::npos;
    }

    std::string DataTransHub::MakeDataFile(const std::string &dir, const std::string &prefix,
                                           const std::string &lastFilePath) {
        std::string ret;
        std::size_t index = GetIndex(lastFilePath);
        ret.append(dir).append("/").append(prefix).append("_").append(
                TimeUtil::GetFormatDate("%Y-%m-%d-%H-%M")).append("_").append(
                Int64ToStr(index + 1)).append(DATA_SUFFIX);
        SafeIO safeIo("", "", "data_hub");
        FILE *fp = safeIo.OpenForAppend(ret, "ab+");
        if (fp != nullptr) {
            safeIo.CloseForAppend(fp);
        }
        return ret;
    }

    std::shared_ptr<std::list<std::string> >
    DataTransHub::GetOrderedDataFiles(const std::string &dir) {
        std::shared_ptr<std::list<std::string> > files = File::FileList(dir);
        if (!files->empty()) {
            files = File::Filter(*files, [](const std::string &filePath) {
                if (EndsWith(filePath, DATA_SUFFIX)) {
                    return true;
                }
                return false;
            });

            files->sort([this](std::string str1, std::string str2) {
                std::string str1Name = File::GetFileName(str1);
                std::string str2Name = File::GetFileName(str2);
                std::size_t index1 = GetIndex(str1Name);
                std::size_t index2 = GetIndex(str2Name);
                if (index1 < index2) {
                    return true;
                }
                return false;
            });
        }
        return files;
    }

    std::string DataTransHub::GetLastDataFile(const std::string &dir, const std::string &prefix) {
        std::shared_ptr<std::list<std::string> > files = GetOrderedDataFiles(dir);
        std::string dataFilePath;
        if (!files->empty()) {
            dataFilePath = files->rbegin().operator*();
            dataFilePath = dir + "/" + dataFilePath;
        }

        if (dataFilePath.empty()) {
            dataFilePath.append(dir).append("/").append(prefix).append("_").append(
                    TimeUtil::GetFormatDate("%Y-%m-%d-%H-%M")).append("_").append(
                    Int64ToStr(0)).append(DATA_SUFFIX);
        }
        return dataFilePath;
    }

    std::string DataTransHub::GetWriteFile(const std::string &dir, const std::string &prefix,
                                           std::size_t writeSize) {
        std::string filePath = GetLastDataFile(dir, prefix);
        FileInfo fileInfo = File::GetFileInfo(filePath);
        if (fileInfo.Size + writeSize < m_MaxFileSize) {
            return filePath;
        }

        filePath = MakeDataFile(dir, prefix, filePath);
        return filePath;
    }

    bool DataTransHub::IsExpired(std::int64_t date, std::int64_t expiredTime) {
        if (expiredTime == 0 || date == 0) {
            return false;
        }
        std::int64_t now = TimeUtil::GetSystemClockSecondsTime();
        std::int64_t deadLine = date + expiredTime;
        if (now > deadLine) {
            return true;
        }
        return false;
    }

    void DataTransHub::DeleteExpiredFile(const std::string &dir) {
        std::shared_ptr<std::list<std::string> > files = GetOrderedDataFiles(dir);
        std::string filePath;

        bool isRectify = false;
        if (files->rbegin() != files->rend()) {
            isRectify = IsRectifyFileName(files->rbegin().operator*());
        }

        int i = 0;
        for (std::list<std::string>::iterator iter = files->begin(); iter !=
                                                                     files->end();
             iter++) {
            filePath.clear();
            filePath.append(dir).append("/").append(*iter);
            FileInfo fileInfo = File::GetFileInfo(filePath);
            if (IsExpired(fileInfo.MTime, m_ExpiredTime)) {
                File::RemoveFile(filePath);
                continue;
            }
            if (isRectify) {
                RectifyFileName(filePath, i++);
            }
        }
    }

    void DataTransHub::WriteToFile(AutoBuffer &buffer) {
        std::lock_guard<std::mutex> lk(m_WriteFileMut);
        if (buffer.Length() <= 0) {
            return;
        }
        std::string filePath = GetWriteFile(m_DataDir, m_Prefix, buffer.Length());
        SafeIO safeIo("", "", "data_hub");
        FILE *fp = safeIo.OpenForAppend(filePath, "ab+");
        if (fp != nullptr) {
            safeIo.Write(buffer.Ptr(), buffer.Length(), fp);
            safeIo.CloseForAppend(fp);
        }

        m_ThreadManager->CallOnMainThread([this]() {
            DeleteExpiredFile(m_DataDir);
        });
    }

    void DataTransHub::WriteDiskTask() {
        std::lock_guard<std::recursive_mutex> lk(m_Mut);
        AutoBuffer buffer;
        m_LogBuffer->Flush(buffer);
        WriteToFile(buffer);
    }

    void DataTransHub::Restore() {
        std::shared_ptr<AutoBuffer> buffer = std::make_shared<AutoBuffer>();
        m_LogBuffer->Flush(*buffer);
        if (buffer->Length() > 0) {
            m_ThreadManager->CallOnMainThread([this, buffer]() {
                WriteToFile(*buffer);
            });
        }
    }

    bool DataTransHub::IsRectifyFileName(const std::string &filePath) {
        std::size_t index = GetIndex(filePath);
        if (index >= MAX_INDEX) {
            return true;
        }
        return false;
    }

    std::string DataTransHub::RectifyFileName(const std::string &filePath, std::size_t index) {
        std::size_t pos = GetIndexPos(filePath);
        if (pos != std::string::npos) {
            std::string newFileName = filePath.substr(0, pos + 1) + Int64ToStr(index);
            if (File::ReName(filePath, newFileName)) {
                return newFileName;
            }
        }
        return filePath;
    }

    void DataTransHub::Upload() {
        if (!m_UploadImp) {
            return;
        }
        WriteDiskTask();

        std::shared_ptr<std::list<std::string> > files = GetOrderedDataFiles(m_DataDir);
        std::string filePath;
        for (std::list<std::string>::iterator iter = files->begin(); iter != files->end(); iter++) {
            filePath.clear();
            filePath.append(m_DataDir).append("/").append(*iter);
            FileInfo fileInfo = File::GetFileInfo(filePath);
            if (IsExpired(fileInfo.MTime, m_ExpiredTime)) {
                File::RemoveFile(filePath);
                continue;
            }

            if (fileInfo.Size > 0) {
                std::list<std::string>::iterator lastIter = iter;
                lastIter++;
                if (lastIter == files->end()) {
                    if (IsRectifyFileName(filePath)) {
                        filePath = RectifyFileName(filePath, 0);
                    }
                    MakeDataFile(m_DataDir, m_Prefix, filePath);
                }
                m_UploadDelayTasks->Stop();
                m_UploadImp(filePath);
            }
            break;
        }
    }

    void DataTransHub::SetFileMaxSize(std::size_t fileMaxSize) {
        std::lock_guard<std::recursive_mutex> lk(m_Mut);
        if (m_IsStart) {
            return;
        }

        if (fileMaxSize <= 0) {
            return;
        }
        m_ThreadManager->CallOnMainThread([this, fileMaxSize]() {
            m_MaxFileSize = fileMaxSize;
        });
    }

    void DataTransHub::SetBufferSize(std::size_t bufferSize) {
        std::lock_guard<std::recursive_mutex> lk(m_Mut);
        if (m_IsStart) {
            return;
        }
        if (bufferSize <= 0) {
            return;
        }

        m_ThreadManager->CallOnMainThread([this, bufferSize]() {
            m_BufferSize = bufferSize;
        });
    }

    void DataTransHub::SetExpiredTime(std::int64_t expiredTime) {
        std::lock_guard<std::recursive_mutex> lk(m_Mut);
        if (m_IsStart) {
            return;
        }
        if (expiredTime < 0) {
            return;
        }

        m_ThreadManager->CallOnMainThread([this, expiredTime]() {
            m_ExpiredTime = expiredTime;
        });
    }

    void DataTransHub::SetReportingInterval(std::int64_t reportingInterval) {
        std::lock_guard<std::recursive_mutex> lk(m_Mut);
        if (m_IsStart) {
            return;
        }
        if (reportingInterval < 0) {
            return;
        }

        m_ThreadManager->CallOnMainThread([this, reportingInterval]() {
            m_ReportingInterval = reportingInterval;
        });
    }

    void DataTransHub::SetRetryInterval(std::int64_t retryInterval) {
        std::lock_guard<std::recursive_mutex> lk(m_Mut);
        if (m_IsStart) {
            return;
        }
        if (retryInterval < 0) {
            return;
        }

        m_ThreadManager->CallOnMainThread([this, retryInterval]() {
            m_Step = retryInterval;
        });
    }

    void DataTransHub::SetDataFilePrefix(const std::string &prefix) {
        std::lock_guard<std::recursive_mutex> lk(m_Mut);
        if (m_IsStart) {
            return;
        }
        m_ThreadManager->CallOnMainThread([this, prefix]() {
            m_Prefix = prefix;
        });
    }

    void DataTransHub::SetUploadTriggerWayWay(UploadTriggerWay uploadTriggerWay) {
        std::lock_guard<std::recursive_mutex> lk(m_Mut);
        if (m_IsStart) {
            return;
        }
        m_ThreadManager->CallOnMainThread([this, uploadTriggerWay]() {
            m_UploadTriggerWay = uploadTriggerWay;
        });
    }

    void DataTransHub::SetWriteDiskPeriod(std::size_t period) {
        std::lock_guard<std::recursive_mutex> lk(m_Mut);
        if (m_IsStart) {
            return;
        }
        m_ThreadManager->CallOnMainThread([this, period]() {
            m_WriteTaskPeriod = period;
        });
    }

    void DataTransHub::Start() {
        std::lock_guard<std::recursive_mutex> lk(m_Mut);
        if (m_IsStart) {
            return;
        }

        m_ThreadManager->CallOnMainThread([this]() {
            std::function<void(void)> uploadTask = std::bind(&DataTransHub::Upload, this);
            m_UploadDelayTasks = std::make_shared<Timer>(std::move(uploadTask), 0,
                                                         m_ReportingInterval);
            std::function<void(void)> writeDiskTask = std::bind(&DataTransHub::WriteDiskTask, this);
            m_WriteDiskTasks = std::make_shared<Timer>(std::move(writeDiskTask), 0,
                                                       m_WriteTaskPeriod);

            if (m_UploadTriggerWay == UploadTriggerWay::Auto) {
                m_UploadDelayTasks->StartWithPeriodTaskManager(m_PeriodTaskManager, true);
            }

            if (m_WriteTaskPeriod != 0) {
                m_WriteDiskTasks->StartWithPeriodTaskManager(m_PeriodTaskManager, true);
            }
        });

        Open();
        Restore();
        m_ThreadManager->Start();
        m_IsStart = true;
    }

    void DataTransHub::ReaWaken() {
        if (!m_ThreadManager->IsMainThread()) {
            m_ThreadManager->CallOnMainThread([this]() {
                ReaWaken();
            });
            return;
        }
        if (!m_IsStart) {
            return;
        }

        m_NextUploadTime = 0;
        if (m_UploadDelayTasks->IsActive()) {
            m_UploadDelayTasks->Stop();
        }
        m_UploadDelayTasks->ReStartWithPeriodTaskManager(m_PeriodTaskManager, m_NextUploadTime);
    }

    void DataTransHub::ManualTriggerUpload(std::function<void(void)> finish) {
        if (!m_ThreadManager->IsMainThread()) {
            m_ThreadManager->CallOnMainThread([this, finish]() {
                ManualTriggerUpload(finish);
            });
            return;
        }
        if (!m_IsStart) {
            return;
        }
        std::string filePath = GetLastDataFile(m_DataDir, m_Prefix);
        m_FileToCallback[filePath] = finish;
        m_UploadDelayTasks->Stop();
        m_UploadDelayTasks->ReStartWithPeriodTaskManager(m_PeriodTaskManager, 0);

    }

    void DataTransHub::Push(const std::vector<unsigned char> &data) {
        std::lock_guard<std::recursive_mutex> lk(m_Mut);
        if (!m_IsStart) {
            return;
        }

        if (m_LogBuffer->GetData().Length() + data.size() >= (m_BufferSize * 4 / 5)) {
            WriteDiskTask();
        } else if (m_LogBuffer->GetData().Length() + data.size() >= (m_BufferSize * 3 / 5)) {
            std::shared_ptr<AutoBuffer> buffer = std::make_shared<AutoBuffer>();
            m_LogBuffer->Flush(*buffer);
            m_ThreadManager->CallOnMainThread([this, buffer]() {
                WriteToFile(*buffer);
            });
        }

        m_LogBuffer->Write(data.data(), data.size());
        m_LogBuffer->Write("\n", 1);
    }

    void DataTransHub::NotifyUploadSuccess(std::string filePath) {
        if (!m_ThreadManager->IsMainThread()) {
            m_ThreadManager->CallOnMainThread([this, filePath]() {
                NotifyUploadSuccess(filePath);
            });
            return;
        }
        if (!m_IsStart) {
            return;
        }

        m_NextUploadTime = 0;
        File::RemoveFile(filePath);
        std::map<std::string, std::function<void(void)>>::iterator iter = m_FileToCallback.find(
                filePath);
        if (iter != m_FileToCallback.end()) {
            iter->second();
            m_FileToCallback.erase(iter);
            if (m_UploadTriggerWay == UploadTriggerWay::Manual) {
                return;
            }
        }

        if (m_UploadDelayTasks->IsActive()) {
            m_UploadDelayTasks->Stop();
        }
        m_UploadDelayTasks->StartWithPeriodTaskManager(m_PeriodTaskManager, true);
    }

    void DataTransHub::NotifyUploadFailed(std::string filePath) {
        if (!m_ThreadManager->IsMainThread()) {
            m_ThreadManager->CallOnMainThread([this, filePath]() {
                NotifyUploadFailed(filePath);
            });
            return;
        }
        if (!m_IsStart) {
            return;
        }

        if (m_NextUploadTime < MAX_WAIT_TIME) {
            m_NextUploadTime += m_Step;
        }
        m_UploadDelayTasks->ReStartWithPeriodTaskManager(m_PeriodTaskManager, m_NextUploadTime);
    }
}
