#ifndef NET_DISPATCHER_NEVER_DESTROYED_H
#define NET_DISPATCHER_NEVER_DESTROYED_H

#include "class_property.h"

namespace future {
    template<typename T> class NeverDestroyed {
        NONCOPYABLE(NeverDestroyed);
        FORBID_HEAP_ALLOCATION;
    public:
        template<typename... Args> NeverDestroyed(Args&&... args) {
            new (StoragePointer()) T(std::forward<Args>(args)...);
        }
        
        NeverDestroyed(NeverDestroyed&& other) {
            new (StoragePointer()) T(std::move(*other.StoragePointer()));
        }
        
        operator T&() { return *StoragePointer(); }
        T& get() { return *StoragePointer(); }
        
        T* operator->() { return StoragePointer(); }
        
        operator const T&() const { return *StoragePointer(); }
        const T& get() const { return *StoragePointer(); }
        
        const T* operator->() const { return StoragePointer(); }
        
    private:
        using PointerType = typename std::remove_const<T>::type*;
        
        PointerType StoragePointer() const { return const_cast<PointerType>(reinterpret_cast<const T*>(&storage_)); }
    
        typename std::aligned_storage<sizeof(T), std::alignment_of<T>::value>::type storage_;
    };
}

#endif //NET_DISPATCHER_NEVER_DESTROY_H
