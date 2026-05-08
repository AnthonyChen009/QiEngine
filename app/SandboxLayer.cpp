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
    // move
    m_quad.position += m_quad.velocity * (float)ts;
    //m_quad.rotation += 90.0f * ts;

    // get window bounds
    float halfW = 1280.0f / 2.0f;
    float halfH = 720.0f / 2.0f;
    float halfSizeX = m_quad.size.x / 2.0f;
    float halfSizeY = m_quad.size.y / 2.0f;

    // bounce
    if (m_quad.position.x + halfSizeX >= halfW || m_quad.position.x - halfSizeX <= -halfW) {
        m_quad.velocity.x *= -1.0f;

    }
    if (m_quad.position.y + halfSizeY >= halfH || m_quad.position.y - halfSizeY <= -halfH) {
        m_quad.velocity.y *= -1.0f;

    }

    Qi::Renderer2D::drawQuad(m_quad.position, m_quad.size, m_quad.rotation, m_quad.color);
}
void SandboxLayer::onEvent(Qi::Event& e) {
}
