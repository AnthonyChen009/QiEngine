#include "Application.hpp"
#include "Window.hpp"
#include <iostream>
#include <QiEngine.hpp>
#include <memory>

class Sandbox : public Qi::Application {
public:
    Sandbox(const Qi::ApplicationSpecification& specification) : Qi::Application(specification){

    }
    ~Sandbox() {

    }
};

std::unique_ptr<Qi::Application> Qi::createApplication(Qi::ApplicationCommandLineArgs args) {
    ApplicationSpecification spec;
    spec.name = "Sandbox";
    spec.windowWidth = 1280;
    spec.windowHeight = 720;
    spec.graphicsAPI = GraphicsAPI::Vulkan;
    spec.workingDirectory = QI_ASSET_PATH;
    spec.commandLineArgs = args;
    return std::make_unique<Sandbox>(spec);
}
