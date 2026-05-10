#include "SandboxLayer.hpp"
#include "QiEngine.hpp"


SandboxLayer::SandboxLayer()
    : Layer("SandboxLayer")
{
}

void SandboxLayer::onAttach() {
    m_scene = QiNew<SandboxScene>();
    m_scene->onReady();
}

void SandboxLayer::onDetach() {
    QiDelete(m_scene);
    m_scene = nullptr;
}

void SandboxLayer::onUpdate(Qi::Timestep ts) {
    m_scene->onTick(ts);
}
void SandboxLayer::onEvent(Qi::Event& e) {
    m_scene->onEvent(e);
}
