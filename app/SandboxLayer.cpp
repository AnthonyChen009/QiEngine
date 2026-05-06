#include "SandboxLayer.hpp"
#include "Log.hpp"
#include "QiEngine.hpp"


SandboxLayer::SandboxLayer()
    : Layer("SandboxLayer")
{
}

void SandboxLayer::onAttach() {
}

void SandboxLayer::onDetach() {
}

void SandboxLayer::onUpdate(Qi::Timestep ts) {
    Qi::Renderer::drawQuad();
}

void SandboxLayer::onEvent(Qi::Event& e) {
}
