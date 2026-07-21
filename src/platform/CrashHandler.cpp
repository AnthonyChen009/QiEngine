#include "CrashHandler.hpp"
#include "core/Log.hpp"
#include <cpptrace/cpptrace.hpp>
#include <csignal>
#include <cstdlib>
#include <fstream>
#include <filesystem>
#include <chrono>
#include <sstream>
#include <iomanip>
#include "core/FileSystem.hpp"
#include "SDL3/SDL.h"

#if defined(QI_PLATFORM_WINDOWS)
    #include <Windows.h>
#endif

namespace Qi::CrashHandler {

static std::string s_customCrashReason;

[[noreturn]] void triggerCrash(const std::string& reason) {
    s_customCrashReason = reason;
    std::abort();
}

static std::string getTimestampString() {
    auto now = std::chrono::system_clock::now();
    std::time_t nowTime = std::chrono::system_clock::to_time_t(now);
    std::tm tmBuf{};
#if defined(QI_PLATFORM_WINDOWS)
    localtime_s(&tmBuf, &nowTime);
#else
    localtime_r(&nowTime, &tmBuf);
#endif
    std::ostringstream oss;
    oss << std::put_time(&tmBuf, "%Y%m%d_%H%M%S");
    return oss.str();
}

static void writeCrashReport(const char* name) {
    std::filesystem::path crashDir = FileSystem::getExecutablePath() / "crash_logs";
    std::filesystem::create_directories(crashDir);

    std::string filename = "crash_" + getTimestampString() + ".log";
    std::filesystem::path crashFile = crashDir / filename;

    std::ofstream file(crashFile, std::ios::app);
    file << "=== CRASH: " << name << " ===\n";
    file << "Timestamp: " << getTimestampString() << "\n\n";
    file << cpptrace::generate_trace().to_string();
    file.flush();
    file.close();

    QI_CORE_ERROR("=== CRASH DETECTED: {} === (see {})", name, crashFile.string());
    cpptrace::generate_trace().print();
    spdlog::default_logger()->flush();

    std::string message = std::string("QiEngine crashed: ") + name + "\n\nCrash log saved to:\n" + crashFile.string();
    SDL_ShowSimpleMessageBox(SDL_MESSAGEBOX_ERROR, "QiEngine Crashed", message.c_str(), nullptr);
}

#if defined(QI_PLATFORM_WINDOWS)

static const char* exceptionCodeToString(DWORD code) {
    switch (code) {
        case EXCEPTION_ACCESS_VIOLATION:      return "EXCEPTION_ACCESS_VIOLATION";
        case EXCEPTION_STACK_OVERFLOW:        return "EXCEPTION_STACK_OVERFLOW";
        case EXCEPTION_INT_DIVIDE_BY_ZERO:    return "EXCEPTION_INT_DIVIDE_BY_ZERO";
        case EXCEPTION_ILLEGAL_INSTRUCTION:   return "EXCEPTION_ILLEGAL_INSTRUCTION";
        case EXCEPTION_ARRAY_BOUNDS_EXCEEDED: return "EXCEPTION_ARRAY_BOUNDS_EXCEEDED";
        case EXCEPTION_FLT_DIVIDE_BY_ZERO:    return "EXCEPTION_FLT_DIVIDE_BY_ZERO";
        default:                              return "UNKNOWN_EXCEPTION";
    }
}

static LONG WINAPI unhandledExceptionFilter(EXCEPTION_POINTERS* exceptionInfo) {
    writeCrashReport(exceptionCodeToString(exceptionInfo->ExceptionRecord->ExceptionCode));
    return EXCEPTION_EXECUTE_HANDLER;
}

static void abortHandler(int) {
    const char* reason = s_customCrashReason.empty() ? "SIGABRT (Abort)" : s_customCrashReason.c_str();
    writeCrashReport(reason);
    std::signal(SIGABRT, SIG_DFL);
    std::raise(SIGABRT);
}

void init() {
    ULONG stackGuarantee = 64 * 1024;
    SetThreadStackGuarantee(&stackGuarantee);

    SetUnhandledExceptionFilter(unhandledExceptionFilter);
    std::signal(SIGABRT, abortHandler);

    QI_CORE_INFO("Crash handler initialized");
}

#else // Linux/POSIX

static std::vector<char> s_altStack(SIGSTKSZ * 4);

static void signalHandler(int sig) {
    const char* name = "UNKNOWN";
    switch (sig) {
        case SIGSEGV: name = "SIGSEGV (Segmentation Fault)"; break;
        case SIGABRT: name = s_customCrashReason.empty() ? "SIGABRT (Abort)" : s_customCrashReason.c_str(); break;
        case SIGFPE:  name = "SIGFPE (Floating Point Exception)"; break;
        case SIGILL:  name = "SIGILL (Illegal Instruction)"; break;
        case SIGBUS:  name = "SIGBUS (Bus Error)"; break;
    }

    writeCrashReport(name);
    s_customCrashReason.clear();

    std::signal(sig, SIG_DFL);
    std::raise(sig);
}

void init() {
    stack_t ss{};
    ss.ss_sp = s_altStack.data();
    ss.ss_size = s_altStack.size();
    ss.ss_flags = 0;
    sigaltstack(&ss, nullptr);

    struct sigaction sa{};
    sa.sa_handler = signalHandler;
    sa.sa_flags = SA_ONSTACK;
    sigemptyset(&sa.sa_mask);

    sigaction(SIGSEGV, &sa, nullptr);
    sigaction(SIGABRT, &sa, nullptr);
    sigaction(SIGFPE, &sa, nullptr);
    sigaction(SIGILL, &sa, nullptr);
    sigaction(SIGBUS, &sa, nullptr);

    QI_CORE_INFO("Crash handler initialized");
}

#endif

}
