#include "SandboxScene.hpp"
#include "QiEngine.hpp"
#include "core/Application.hpp"
#include "core/KeyCodes.hpp"
#include "core/input/Input.hpp"
#include "os/Memory.hpp"

#include "renderer/vulkan/Texture2D.hpp"
#include "scene/2d/Sprite2D.hpp"
#include "scene/Components.hpp"
#include "QiEngine.hpp"
#include <glm/ext/vector_float2.hpp>

void SandboxScene::onReady() {
    m_player = QiNew<Qi::Sprite2D>();
    addNode(m_player);
    m_player->addComponent<Qi::RigidbodyComponent>();
    m_player->setTexture(Qi::Application::get().getResourceLoader().Load<Qi::Texture2D>("images/textureTest.jpg"));
}

void SandboxScene::onUpdate(Qi::Timestep ts) {
    glm::vec2& velocity = m_player->getComponent<Qi::RigidbodyComponent>().velocity;
    glm::vec2 direction{0.0f, 0.0f};
    if (Qi::Input::isKeyPressed(Qi::Key::W)) direction.y += 1.0f;
    if (Qi::Input::isKeyPressed(Qi::Key::S)) direction.y -= 1.0f;
    if (Qi::Input::isKeyPressed(Qi::Key::A)) direction.x -= 1.0f;
    if (Qi::Input::isKeyPressed(Qi::Key::D)) direction.x += 1.0f;

    float speed = 100.0f;
    if (direction.x != 0.0f || direction.y != 0.0f) {
        direction = glm::normalize(direction);
    }
    velocity = direction * speed;
}

void SandboxScene::onEvent(Qi::Event& event) {
    Scene::onEvent(event);
}
