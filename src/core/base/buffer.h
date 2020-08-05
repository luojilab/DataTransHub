#ifndef BUFFER_H
#define BUFFER_H

#include <memory>
#include <cstdint>
#include <cstddef>
#include <string.h>

namespace future {

    enum BufferCopyFlag {
        BufferCopy,
        BufferNoCopy,
    };

    class Buffer {
    private:
        void *m_Ptr;
        size_t m_Size;
        BufferCopyFlag m_IsNoCopy;

    public:
        void *GetBegin() const { return m_Ptr; }

        void *GetEnd() const { return (unsigned char *) m_Ptr + m_Size; }

        size_t Length() const { return m_Size; }

        void Clear() {
            memset(m_Ptr, 0, m_Size);
        }

        Buffer(size_t length = 0);

        Buffer(void *source, size_t length, BufferCopyFlag noCopy = BufferCopy);

        Buffer(Buffer &&other) noexcept;

        Buffer &operator=(Buffer &&other) noexcept;

        ~Buffer();

    private:
        // those are expensive, just forbid it for possibly misuse
        Buffer(const Buffer &other) = delete;

        Buffer &operator=(const Buffer &other) = delete;
    };

}
#endif //BUFFER_H
