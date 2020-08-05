#include <cstdlib>
#include <cstring>
#include <utility>
#include "buffer.h"

namespace future {

    Buffer::Buffer(size_t length) : m_Ptr(nullptr), m_Size(length), m_IsNoCopy(BufferCopy) {
        if (m_Size > 0) {
            m_Ptr = malloc(m_Size);
        }
    }

    Buffer::Buffer(void *source, size_t length, BufferCopyFlag noCopy)
            : m_Ptr(source), m_Size(length), m_IsNoCopy(noCopy) {
        if (m_IsNoCopy == BufferCopy) {
            m_Ptr = malloc(m_Size);
            memcpy(m_Ptr, source, m_Size);
        }
    }

    Buffer::Buffer(Buffer &&other) noexcept
            : m_Ptr(other.m_Ptr), m_Size(other.m_Size), m_IsNoCopy(other.m_IsNoCopy) {
        other.m_Ptr = nullptr;
        other.m_Size = 0;
        other.m_IsNoCopy = BufferCopy;
    }

    Buffer &Buffer::operator=(Buffer &&other) noexcept {
        std::swap(m_Ptr, other.m_Ptr);
        std::swap(m_Size, other.m_Size);
        std::swap(m_IsNoCopy, other.m_IsNoCopy);

        return *this;
    }

    Buffer::~Buffer() {
        if (m_IsNoCopy == BufferCopy && m_Ptr) {
            free(m_Ptr);
        }
        m_Ptr = nullptr;
    }

}