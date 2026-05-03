#pragma once

#include "Base.hpp"

#define GLM_ENABLE_EXPERIMENTAL
#include <glm/gtx/string_cast.hpp>

// Ignore warnings in external headers (MSVC)
#ifdef _MSC_VER
#pragma warning(push, 0)
#endif

#include <spdlog/spdlog.h>
#include <spdlog/fmt/ostr.h>

#ifdef _MSC_VER
#pragma warning(pop)
#endif

namespace Qi {

class Log {
public:
    static void init();

    static Ref<spdlog::logger>& getCoreLogger() { return s_coreLogger; }
    static Ref<spdlog::logger>& getClientLogger() { return s_clientLogger; }

private:
    static Ref<spdlog::logger> s_coreLogger;
    static Ref<spdlog::logger> s_clientLogger;
};

} // namespace Qi

// glm stream operators
template<typename OStream, glm::length_t L, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::vec<L, T, Q>& v) {
    return os << glm::to_string(v);
}

template<typename OStream, glm::length_t C, glm::length_t R, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::mat<C, R, T, Q>& m) {
    return os << glm::to_string(m);
}

template<typename OStream, typename T, glm::qualifier Q>
inline OStream& operator<<(OStream& os, const glm::qua<T, Q>& q) {
    return os << glm::to_string(q);
}

// Core log macros
#define QI_CORE_TRACE(...)    ::Qi::Log::getCoreLogger()->trace(__VA_ARGS__)
#define QI_CORE_INFO(...)     ::Qi::Log::getCoreLogger()->info(__VA_ARGS__)
#define QI_CORE_WARN(...)     ::Qi::Log::getCoreLogger()->warn(__VA_ARGS__)
#define QI_CORE_ERROR(...)    ::Qi::Log::getCoreLogger()->error(__VA_ARGS__)
#define QI_CORE_CRITICAL(...) ::Qi::Log::getCoreLogger()->critical(__VA_ARGS__)

// Client log macros
#define QI_TRACE(...)    ::Qi::Log::getClientLogger()->trace(__VA_ARGS__)
#define QI_INFO(...)     ::Qi::Log::getClientLogger()->info(__VA_ARGS__)
#define QI_WARN(...)     ::Qi::Log::getClientLogger()->warn(__VA_ARGS__)
#define QI_ERROR(...)    ::Qi::Log::getClientLogger()->error(__VA_ARGS__)
#define QI_CRITICAL(...) ::Qi::Log::getClientLogger()->critical(__VA_ARGS__)
