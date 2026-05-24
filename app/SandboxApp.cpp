#include "core/Application.hpp"
#include "SandboxLayer.hpp"
#include "core/Window.hpp"
#include <iostream>
#include <QiEngine.hpp>
#include "core/EntryPoint.hpp"
#include <memory>
#include <iostream>

class Sandbox : public Qi::Application {
public:
    Sandbox(const Qi::ApplicationSpecification& specification) : Qi::Application(specification){
        pushLayer(new SandboxLayer());
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
