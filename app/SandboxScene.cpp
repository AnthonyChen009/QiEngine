#include "SandboxScene.hpp"
#include "QiEngine.hpp"
#include "core/Application.hpp"
#include "core/KeyCodes.hpp"
#include "core/api/MeshPrimitives.hpp"
#include "core/io/ResourceLoader.hpp"
#include "os/Memory.hpp"

#include "renderer/vulkan/Mesh.hpp"
#include "renderer/vulkan/Texture2D.hpp"
#include "scene/2d/Sprite2D.hpp"
#include "scene/3d/Camera3D.hpp"
#include "scene/3d/MeshInstance3D.hpp"
#include "scene/3d/Node3D.hpp"
#include "scene/Components.hpp"
#include "QiEngine.hpp"
#include <glm/ext/vector_float2.hpp>

void SandboxScene::onReady() {
    m_player = QiNew<Qi::Sprite2D>();
    addNode(m_player);
    m_player->setSize({10.0f, 10.0f});
    //m_player->setTexture(Qi::Application::get().getResourceLoader().Load<Qi::Texture2D>("images/house-RGB.png"));

    // m_testNode = QiNew<Qi::MeshInstance3D>();
    // addNode(m_testNode);
    // m_testNode->setMesh(Qi::MeshPrimitives::CapsuleMesh());
    // m_testNode->addComponent<Qi::Rigidbody3DComponent>();

    // m_testNode2 = QiNew<Qi::MeshInstance3D>();
    // addNode(m_testNode2);
    // m_testNode2->setMesh(Qi::Application::get().getResourceLoader().Load<Qi::Mesh>("models/Chicken.obj"));
    // m_testNode2->setPosition({0,0,0});
    // m_testNode2->getComponent<Qi::MeshComponent>().albedoTexture = Qi::Application::get().getResourceLoader().Load<Qi::Texture2D>("images/75.png").get();

    m_camera = QiNew<CameraController>();
    addNode(m_camera);
    m_camera->setPosition({0.0f, 2.0f, 2.0f});
    setActiveCamera(*m_camera);

    m_directionLight = QiNew<Qi::Node3D>();
    addNode(m_directionLight);
    m_directionLight->addComponent<Qi::DirectionalLightComponent>();
}

void SandboxScene::onUpdate(Qi::Timestep ts) {
    Qi::Scene::onUpdate(ts);
    //m_turntableAngle += glm::radians(45.0f) * ts.getSeconds();
    //m_testNode2->setRotation(glm::angleAxis(m_turntableAngle, glm::vec3(0, 1, 0)));
}

void SandboxScene::onEvent(Qi::Event& event) {
    Scene::onEvent(event);
}
