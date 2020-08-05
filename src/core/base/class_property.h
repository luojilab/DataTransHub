#ifndef NET_DISPATCHER_CLASS_PROPERTY_H
#define NET_DISPATCHER_CLASS_PROPERTY_H

#define FORBID_HEAP_ALLOCATION \
private: \
    void* operator new(size_t, void*) = delete; \
    void* operator new[](size_t, void*) = delete; \
    void* operator new(size_t) = delete; \
    void* operator new[](size_t size) = delete;

#define NONCOPYABLE(ClassName) \
    ClassName(const ClassName&) = delete; \
    ClassName& operator=(const ClassName&) = delete; \

#endif //NET_DISPATCHER_CLASS_PROPERTY_H
