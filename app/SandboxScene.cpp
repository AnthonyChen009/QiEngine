#include "SandboxScene.hpp"
#include "QiEngine.hpp"
#include "os/Memory.hpp"
#include "scene/2d/Sprite2D.hpp"

void SandboxScene::onReady() {
    m_player = QiNew<Qi::Sprite2D>("Player");
    addNode(m_player, 0);
    m_player->setTexture("images/textureTest.jpg");
    m_player->setSize(glm::vec2(100, 100));
    m_player->addComponent<Qi::RigidbodyComponent>(glm::vec2{ 200.0f, 150.0f });
    m_child = QiNew<Qi::Sprite2D>("Child");
    m_player->addChild(m_child);
    m_child->setSize(glm::vec2(100, 100));
    m_child->setPosition(glm::vec2(100, 100));
}

void SandboxScene::onUpdate(Qi::Timestep ts) {
    Qi::TransformComponent& transform = m_player->getComponent<Qi::TransformComponent>();
    Qi::RigidbodyComponent& rb = m_player->getComponent<Qi::RigidbodyComponent>();

    float halfW = 1280.0f / 2.0f;
    float halfH = 720.0f / 2.0f;
    float halfSizeX = transform.size.x / 2.0f;
    float halfSizeY = transform.size.y / 2.0f;

    if (transform.position.x + halfSizeX >= halfW) {
        rb.velocity.x *= -1.0f;
        transform.position.x = halfW - halfSizeX;
    } else if (transform.position.x - halfSizeX <= -halfW) {
        rb.velocity.x *= -1.0f;
        transform.position.x = -halfW + halfSizeX;
    }

    if (transform.position.y + halfSizeY >= halfH) {
        rb.velocity.y *= -1.0f;
        transform.position.y = halfH - halfSizeY;
    } else if (transform.position.y - halfSizeY <= -halfH) {
        rb.velocity.y *= -1.0f;
        transform.position.y = -halfH + halfSizeY;
    }

}

void SandboxScene::onEvent(Qi::Event& e) {
    Scene::onEvent(e);
}
