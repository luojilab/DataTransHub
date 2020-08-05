#ifndef MMAPEDFILE_H
#define MMAPEDFILE_H

#include <string>
#include <sys/ioctl.h>

namespace future {

    extern const int DEFAULT_MMAP_SIZE;

    class MmapedFile {

    public:
        MmapedFile(const std::string &path,
                   size_t size = static_cast<size_t>(DEFAULT_MMAP_SIZE));

        ~MmapedFile();

        bool Open();

        void Close();

        void Sycn();

        void Clear();

        bool IsOpened();

        size_t GetMaxSize() { return m_SegmentSize; }

        void *GetMemBegin() { return m_SegmentPtr; }

        void *GetMemEnd() { return (unsigned char *)m_SegmentPtr + m_SegmentSize; }

        const std::string &GetPath() { return m_Path; }

    private:
        // just forbid it for possibly misuse
        MmapedFile(const MmapedFile &other) = delete;

        MmapedFile &operator=(const MmapedFile &other) = delete;

    private:
        const std::string m_Path;
        int m_Fd;
        void *m_SegmentPtr;
        size_t m_SegmentSize;
        bool m_IsOpened;
    };

}
#endif //MMAPEDFILE_H
