#pragma once
#include <string>

namespace Qi::CrashHandler {
    void init();
    [[noreturn]] void triggerCrash(const std::string& reason);
}
