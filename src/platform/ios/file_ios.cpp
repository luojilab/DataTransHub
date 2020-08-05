//
// Created by bingjian on 2018/11/5.
//
#include <fcntl.h>
#include <sys/stat.h>
#include <unistd.h>
#include <dirent.h>
#include <set>
#include <libgen.h>
#include "file.h"

#ifndef ACCESSPERMS
/* Permission macros provided by glibc for compatibility with BSDs. */
#define ACCESSPERMS (S_IRWXU | S_IRWXG | S_IRWXO) /* 0777 */
#endif

#ifndef ALLPERMS
#define ALLPERMS    (S_ISUID | S_ISGID | S_ISVTX | S_IRWXU | S_IRWXG | S_IRWXO) /* 07777 */
#endif

#ifndef DEFFILEMODE
#define DEFFILEMODE (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH) /* 0666 */
#endif

namespace future {

    bool File::MkPath(const std::string &path) {

        size_t nPos = 0, nIndex = 0;
        std::string strPath = path;
        std::string strSub;
        int ret = 0;
        while ((nPos = strPath.find('/', nIndex)) != std::string::npos) {
            strSub = strPath.substr(0, nPos);
            if (!strSub.empty() && !IsFileExist(strSub)) {
                ret = mkdir(strSub.c_str(), ALLPERMS);
                if (ret != 0) {
                    return false;
                }
            }

            nIndex = nPos + 1;
            while (strPath.length() > nIndex && strPath.at(nIndex) == '/') {
                strPath.replace(nPos, 2, "/");
            }

            if (strPath.length() <= nIndex) {
                break;
            }
        }
        if (strSub.compare(strPath) != 0 && !IsFileExist(strPath)) {
            ret = mkdir(strPath.c_str(), ALLPERMS);
            if (ret != 0) {
                return false;
            }
        }

        return true;
    }

    std::string File::DirName(const std::string &path) {
        char *name = dirname((char *)path.c_str());
        std::string ret(name);
        return ret;
    }

    FileInfo File::GetFileInfo(const std::string &path) {
        FileInfo retInfo;
        if (path.empty()) {
            retInfo.Exists = false;
            retInfo.IsDirectory = false;
            retInfo.Size = 0;
            retInfo.MTime = 0;
            return retInfo;
        }

        struct stat st;
        int ret = 0;

        ret = stat(path.c_str(), &st);
        if (ret != 0) {
            retInfo.Exists = false;
            retInfo.IsDirectory = false;
            retInfo.Size = 0;
            retInfo.MTime = 0;
            return retInfo;
        }
        retInfo.Exists = true;
        retInfo.Size = st.st_size;
        retInfo.MTime = st.st_mtimespec.tv_sec;
        ret = S_ISREG(st.st_mode);
        if (ret != 0) {
            retInfo.IsDirectory = false;
        } else {
            retInfo.IsDirectory = true;
        }
        return retInfo;
    }

    bool File::IsFileExist(const std::string &path) {
        struct stat st;
        int ret = 0;
        ret = stat(path.c_str(), &st);
        if (ret != 0) {
            return false;
        }

        return true;
    }

    bool File::ReName(const std::string &oldPath, const std::string &newPath) {

        int ret = 0;
        ret = rename(oldPath.c_str(), newPath.c_str());
        if (ret != 0) {
            return false;
        }
        return true;
    }

    bool File::RemoveFile(const std::string &path) {

        struct stat st;
        int ret = 0;
        ret = stat(path.c_str(), &st);
        if (ret != 0) {
            return false;
        }

        if (S_ISDIR(st.st_mode)) {
            ret = rmdir(path.c_str());
            if (ret == 0) {
                return true;
            } else {
                return false;
            }
        }

        if (S_ISREG(st.st_mode)) {
            ret = remove(path.c_str());
            if (ret == 0) {
                return true;
            } else {
                return false;
            }
        }
        return false;
    }

    std::shared_ptr<std::list<std::string> > File::FileList(const std::string &path) {
        std::shared_ptr<std::list<std::string> > retList = std::make_shared<std::list<std::string> >();
        DIR *dir = opendir(path.c_str());
        if (NULL == dir) {
            return retList;
        }

        std::string pathTmp;
        struct dirent *file;
        while ((file = readdir(dir)) != NULL) {
            if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0) {
                continue;
            }
            pathTmp.clear();
            pathTmp.append(file->d_name);
            retList->push_back(pathTmp);
        }

        closedir(dir);
        return retList;
    }
}
