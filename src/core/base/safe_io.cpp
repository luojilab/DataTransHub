//
// Created by bingjian on 2019/11/21.
//

#include <list>
#include "safe_io.h"
#include "file.h"
#include "string_util.h"

namespace future {

    SafeIO::SafeIO(const std::string &read_cache_tag,
                   const std::string &write_cache_tag,
                   const std::string &append_tag)
            : read_cache_tag_(read_cache_tag), write_cache_tag_(write_cache_tag),
              append_tag_(append_tag) {
        if (append_tag_.empty()) {
            append_tag_ = "safeio_tag_";
        }
    }

    SafeIO::~SafeIO() {

    }

    FILE *SafeIO::OpenForRead(const std::string &path, const std::string &mode) {
        std::string read_path = path + read_cache_tag_;
        if (File::IsFileExist(read_path)) {
            File::RemoveFile(path);
            File::ReName(read_path, path);
        }
        return fopen(path.c_str(), mode.c_str());
    }

    FILE *SafeIO::OpenForWrite(const std::string &path, const std::string &mode) {
        original_path_ = path;
        write_path_ = path + write_cache_tag_;
        if (File::IsFileExist(write_path_)) {
            File::RemoveFile(write_path_);
        }
        return fopen(write_path_.c_str(), mode.c_str());
    }

    FILE *SafeIO::OpenForAppend(const std::string &path, const std::string &mode) {
        std::string dirName = File::DirName(path);
        std::string pathFileName = File::GetFileName(path);
        std::string recorderName = pathFileName + "_" + append_tag_;
        std::shared_ptr<std::list<std::string> > files = File::FileList(dirName);
        files = File::Filter(*files, [this, recorderName](const std::string &fileName) {
            if (StartsWith(fileName, recorderName)) {
                return true;
            }
            return false;
        });

        FileInfo fileInfo = File::GetFileInfo(path);
        for (std::list<std::string>::iterator iter = files->begin(); iter != files->end(); iter++) {
            std::string fileName = File::GetFileName(*iter);
            std::size_t pos = fileName.find(append_tag_);
            if (pos != std::string::npos) {
                std::string fileSize = fileName.substr(pos);
                int64_t fileSizeInt = StrToInt64(fileSize);
                if (fileInfo.Size < fileSizeInt) {
                    File::Resize(path, fileSizeInt);
                }
            }
            File::RemoveFile(*iter);
        }
        append_path_.clear();
        append_path_.append(dirName).append("/").append(pathFileName).append("_").append(
                append_tag_).append("_").append(Int64ToStr(fileInfo.Size));
        FILE *fp = fopen(append_path_.c_str(), "ab+");
        if (fp == nullptr) {
            return nullptr;
        }
        fclose(fp);

        fp = fopen(path.c_str(), mode.c_str());
        if (fp != nullptr) {
            fseek(fp, fileInfo.Size, SEEK_SET);
        }
        return fp;
    }

    int SafeIO::Seek(FILE *fp, int offset, int whence) {
        return fseek(fp, offset, whence);
    }

    size_t SafeIO::Read(void *buf, size_t count, FILE *fp) {
        return fread(buf, 1u, count, fp);
    }

    size_t SafeIO::Write(void *buf, size_t count, FILE *fp) {
        return fwrite(buf, 1u, count, fp);
    }

    int SafeIO::CloseForRead(FILE *fp) {
        return fclose(fp);
    }

    int SafeIO::CloseForWrite(FILE *fp) {
        int ret = fclose(fp);
        if (ret != 0) {
            return ret;
        }
        if (File::IsFileExist(original_path_)) {
            std::string read_path = original_path_ + read_cache_tag_;
            if (File::IsFileExist(read_path)) {
                File::RemoveFile(read_path);
            }
            File::ReName(write_path_, read_path);
            File::RemoveFile(original_path_);
            File::ReName(read_path, original_path_);
        } else {
            File::ReName(write_path_, original_path_);
        }
        write_path_.clear();
        return 0;
    }

    int SafeIO::CloseForAppend(FILE *fp) {
        int ret = fclose(fp);
        File::RemoveFile(append_path_);
        append_path_.clear();
        append_tag_.clear();
        return ret;
    }
}
