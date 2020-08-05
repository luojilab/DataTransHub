//
// Created by bingjian on 2018/11/1.
//

#ifndef DATA_REPORTER_FILE_H
#define DATA_REPORTER_FILE_H

#include <string>
#include <list>
#include <functional>
#include <fcntl.h>
#include "buffer.h"
#include "file_info.h"

namespace future {
    class File {
    public:

        static bool Resize(const std::string &path, std::size_t size);

        static bool MkPath(const std::string &path);

        static std::string DirName(const std::string &path);

        static bool IsFileExist(const std::string &path);

        static bool ReName(const std::string &oldPath, const std::string &newPath);

        static bool RemoveFile(const std::string &path);

        static bool ZeroFillFile(int fd, size_t startPos, size_t size);

        static FileInfo GetFileInfo(const std::string &path);

        //获取文件名列表，非绝对路径
        static std::shared_ptr<std::list<std::string> > FileList(const std::string &path);

        static std::string GetFileName(const std::string &path);

        static std::string GetFileNameWithExt(const std::string& path);

        static std::shared_ptr<std::list<std::string> >
        Filter(const std::list<std::string> &files,
               std::function<bool(const std::string &fileName)> filter);
    };
}

#endif //DATA_REPORTER_FILE_H
