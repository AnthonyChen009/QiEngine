#pragma once


#include "Application.hpp"
#include "core/Log.hpp"
#include "PlatformDetection.hpp"
#include "FileSystem.hpp"

#ifdef QI_PLATFORM_WINDOWS

extern std::unique_ptr<Qi::Application> Qi::createApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv) {
    Qi::Log::init();

    QI_CORE_ERROR("Logging Init Success");
    Qi::Log::getClientLogger()->info("Hello World!");

    std::unique_ptr<Qi::Application> app = Qi::createApplication({argc, argv});
    app->run();
}

#elif defined(QI_PLATFORM_LINUX)

extern std::unique_ptr<Qi::Application> Qi::createApplication(ApplicationCommandLineArgs args);

int main(int argc, char** argv) {
    Qi::FileSystem::initialize();
    Qi::Log::init();

    QI_CORE_ERROR("Logging Init Success");
    Qi::Log::getClientLogger()->info("Hello World!");

    std::unique_ptr<Qi::Application> app = Qi::createApplication({argc, argv});
    app->run();
}

#else
    #error "Unsupported platform"
#endif
