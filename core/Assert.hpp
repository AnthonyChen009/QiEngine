#pragma once

#include "Base.hpp"
#include "Log.hpp"
#include <filesystem>

#ifdef QI_ENABLE_ASSERTS

	// Alteratively we could use the same "default" message for both "WITH_MSG" and "NO_MSG" and
	// provide support for custom formatting by concatenating the formatting string instead of having the format inside the default message
	#define QI_INTERNAL_ASSERT_IMPL(type, check, msg, ...) { if(!(check)) { QI##type##ERROR(msg, __VA_ARGS__); QI_DEBUGBREAK(); } }
	#define QI_INTERNAL_ASSERT_WITH_MSG(type, check, ...) QI_INTERNAL_ASSERT_IMPL(type, check, "Assertion failed: {0}", __VA_ARGS__)
	#define QI_INTERNAL_ASSERT_NO_MSG(type, check) QI_INTERNAL_ASSERT_IMPL(type, check, "Assertion '{0}' failed at {1}:{2}", QI_STRINGIFY_MACRO(check), std::filesystem::path(__FILE__).filename().string(), __LINE__)

	#define QI_INTERNAL_ASSERT_GET_MACRO_NAME(arg1, arg2, macro, ...) macro
	#define QI_INTERNAL_ASSERT_GET_MACRO(...) QI_EXPAND_MACRO( QI_INTERNAL_ASSERT_GET_MACRO_NAME(__VA_ARGS__, QI_INTERNAL_ASSERT_WITH_MSG, QI_INTERNAL_ASSERT_NO_MSG) )

	// Currently accepts at least the condition and one additional parameter (the message) being optional
	#define QI_ASSERT(...) QI_EXPAND_MACRO( QI_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_, __VA_ARGS__) )
	#define QI_CORE_ASSERT(...) QI_EXPAND_MACRO( QI_INTERNAL_ASSERT_GET_MACRO(__VA_ARGS__)(_CORE_, __VA_ARGS__) )
#else
	#define QI_ASSERT(...)
	#define QI_CORE_ASSERT(...)
#endif
