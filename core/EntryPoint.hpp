#pragma once


#include "Application.hpp"
#include "Log.hpp"
#include "PlatformDetection.hpp"

#ifdef QI_PLATFORM_WINDOWS


#elif defined(QI_PLATFORM_LINUX)

extern std::unique_ptr<Qi::Application> Qi::createApplication();

int main(int argc, char** argv) {
    Qi::Log::init();

    QI_CORE_ERROR("Logging Init Success");
    Qi::Log::getClientLogger()->info("Hello World!");
    std::unique_ptr<Qi::Application> app = Qi::createApplication();
    app->run();
}

#else
    #error "Unsupported platform"
#endif
