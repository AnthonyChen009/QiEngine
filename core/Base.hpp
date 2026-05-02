#pragma once

#include "PlatformDetection.hpp"

#include <memory>
#include <utility>

#ifdef QI_DEBUG
    #if defined(QI_PLATFORM_WINDOWS)
        #define QI_DEBUGBREAK() __debugbreak()
    #elif defined(QI_PLATFORM_LINUX)
        #include <signal.h>
        #define QI_DEBUGBREAK() raise(SIGTRAP)
    #else
        #define QI_DEBUGBREAK()
    #endif
#else
    #define QI_DEBUGBREAK()
#endif

#define QI_EXPAND_MACRO(x) x
#define QI_STRINGIFY_MACRO(x) #x

#define BIT(x) (1 << x)

#define QI_BIND_EVENT_FN(fn) \
    [this](auto&&... args) -> decltype(auto) { \
        return this->fn(std::forward<decltype(args)>(args)...); \
    }

namespace Qi {

    template<typename T>
    using Scope = std::unique_ptr<T>;

    template<typename T, typename... Args>
    constexpr Scope<T> CreateScope(Args&&... args)
    {
        return std::make_unique<T>(std::forward<Args>(args)...);
    }

    template<typename T>
    using Ref = std::shared_ptr<T>;

    template<typename T, typename... Args>
    constexpr Ref<T> CreateRef(Args&&... args)
    {
        return std::make_shared<T>(std::forward<Args>(args)...);
    }

}
