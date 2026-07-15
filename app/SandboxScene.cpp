#include "SandboxScene.hpp"
#include "QiEngine.hpp"
#include "core/Application.hpp"
#include "core/KeyCodes.hpp"
#include "core/api/MeshPrimitives.hpp"
#include "os/Memory.hpp"

#include "renderer/vulkan/Texture2D.hpp"
#include "scene/2d/Sprite2D.hpp"
#include "scene/3d/Camera3D.hpp"
#include "scene/3d/MeshInstance3D.hpp"
#include "scene/Components.hpp"
#include "QiEngine.hpp"
#include <glm/ext/vector_float2.hpp>

void SandboxScene::onReady() {
    m_player = QiNew<Qi::Sprite2D>();
    addNode(m_player);
    m_player->setTexture(Qi::Application::get().getResourceLoader().Load<Qi::Texture2D>("images/textureTest.jpg"));

    m_testNode = QiNew<Qi::MeshInstance3D>();
    addNode(m_testNode);
    m_testNode->setMesh(Qi::MeshPrimitives::CapsuleMesh());
    m_testNode->addComponent<Qi::Rigidbody3DComponent>();

    m_camera = QiNew<Qi::Camera3D>();
    addNode(m_camera);
    m_camera->setPosition({0.0f, 2.0f, 2.0f});
    setActiveCamera(*m_camera);
}

void SandboxScene::onUpdate(Qi::Timestep ts) {
    glm::vec3& velocity = m_testNode->getComponent<Qi::Rigidbody3DComponent>().velocity;

    glm::vec3 direction{0.0f};

    if (Qi::Input::isKeyPressed(Qi::Key::W)) direction.z -= 1.0f;
    if (Qi::Input::isKeyPressed(Qi::Key::S)) direction.z += 1.0f;
    if (Qi::Input::isKeyPressed(Qi::Key::A)) direction.x -= 1.0f;
    if (Qi::Input::isKeyPressed(Qi::Key::D)) direction.x += 1.0f;

    if (Qi::Input::isKeyPressed(Qi::Key::Space)) direction.y += 1.0f;
    if (Qi::Input::isKeyPressed(Qi::Key::LeftControl)) direction.y -= 1.0f;

    float speed = 10.0f;

    if (glm::length(direction) > 0.0f) {
        direction = glm::normalize(direction);
    }

    velocity = direction * speed;
}

void SandboxScene::onEvent(Qi::Event& event) {
    Scene::onEvent(event);
}
