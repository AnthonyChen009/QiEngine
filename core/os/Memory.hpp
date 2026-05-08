#pragma once
#include <cstddef>
#include <unordered_map>
#include <utility>

namespace Qi {

class MemoryTracker {
public:
    static void track(void* ptr, size_t size) {
        s_totalAllocated += size;
        s_allocations[ptr] = size;
    }

    static void untrack(void* ptr) {
        auto it = s_allocations.find(ptr);
        if (it != s_allocations.end()) {
            s_totalAllocated -= it->second;
            s_allocations.erase(it);
        }
    }

    static size_t getTotalAllocated() { return s_totalAllocated; }
    static size_t getAllocationCount() { return s_allocations.size(); }

private:
    static size_t s_totalAllocated;
    static std::unordered_map<void*, size_t> s_allocations;
};
}

template<typename T, typename... Args>
T* QiNew(Args&&... args) {
    T* ptr = new T(std::forward<Args>(args)...);
    Qi::MemoryTracker::track(ptr, sizeof(T));
    return ptr;
}

template<typename T>
void QiDelete(T* ptr) {
    Qi::MemoryTracker::untrack(ptr);
    delete ptr;
}
