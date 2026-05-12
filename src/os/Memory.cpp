#include "Memory.hpp"

namespace Qi {

size_t MemoryTracker::s_totalAllocated = 0;
std::unordered_map<void*, size_t> MemoryTracker::s_allocations;

}
