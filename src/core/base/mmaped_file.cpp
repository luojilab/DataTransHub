#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>
#include "mmaped_file.h"
#include "buffer.h"
#include "log.h"
#include "file.h"


namespace future {

    const int DEFAULT_MMAP_SIZE = getpagesize();

    MmapedFile::MmapedFile(const std::string &path, size_t size)
            : m_Path(path), m_Fd(-1), m_SegmentPtr(nullptr), m_SegmentSize(size),
              m_IsOpened(false) {

    }

    MmapedFile::~MmapedFile() {
        Close();
    }

    bool MmapedFile::Open() {
        m_Fd = open(m_Path.c_str(), O_RDWR | O_CREAT, S_IRWXU);
        if (m_Fd == -1) {
            Error("fail to open:%s, %s", m_Path.c_str(), strerror(errno));
            return false;
        }

        int is_success = ftruncate(m_Fd, m_SegmentSize);
        if (is_success != 0) {
            Error("fail to open:%s, %s", m_Path.c_str(), "ftruncate failed");
            close(m_Fd);
            m_Fd = -1;
            File::RemoveFile(m_Path);
            return false;
        }

        struct stat st = {};
        if (fstat(m_Fd, &st) != -1) {
            m_SegmentSize = static_cast<size_t>(st.st_size);
        }

        if (m_SegmentSize < DEFAULT_MMAP_SIZE) {
            m_SegmentSize = static_cast<size_t>(DEFAULT_MMAP_SIZE);
            if (ftruncate(m_Fd, m_SegmentSize) != 0 ||
                !File::ZeroFillFile(m_Fd, 0, m_SegmentSize)) {
                Error("fail to truncate [%s] to m_Size %zu, %s", m_Path.c_str(),
                      m_SegmentSize, strerror(errno));
                close(m_Fd);
                m_Fd = -1;
                File::RemoveFile(m_Path);
                return false;
            }
        }

        m_SegmentPtr = (char *) mmap(nullptr, m_SegmentSize, PROT_READ | PROT_WRITE, MAP_SHARED,
                                     m_Fd,
                                     0);
        if (m_SegmentPtr == MAP_FAILED) {
            Error("fail to mmap [%s], %s", m_Path.c_str(), strerror(errno));
            close(m_Fd);
            m_Fd = -1;
            m_SegmentPtr = nullptr;
            return false;
        }
        m_IsOpened = true;
        return true;
    }

    void MmapedFile::Close() {
        if (m_SegmentPtr != MAP_FAILED && m_SegmentPtr != nullptr) {
            munmap(m_SegmentPtr, m_SegmentSize);
            m_SegmentPtr = nullptr;
        }

        if (m_Fd != -1) {
            close(m_Fd);
            m_Fd = -1;
        }
        m_IsOpened = false;
    }

    void MmapedFile::Sycn() {
        if (m_SegmentPtr != NULL) {
            if (msync(m_SegmentPtr, m_SegmentSize, MS_SYNC) == -1) {
                Error("fail Sync");
            }
        }
    }

    void MmapedFile::Clear() {
        memset(m_SegmentPtr, 0, m_SegmentSize);
    }

    bool MmapedFile::IsOpened() {
        return m_IsOpened;
    }

}