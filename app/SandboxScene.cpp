#include "SandboxScene.hpp"
#include "QiEngine.hpp"
#include "core/Application.hpp"
#include "core/KeyCodes.hpp"
#include "core/api/MeshPrimitives.hpp"
#include "core/io/ResourceLoader.hpp"
#include "os/Memory.hpp"

#include "renderer/Material.hpp"
#include "renderer/types/MaterialParameters.hpp"
#include "renderer/vulkan/Mesh.hpp"
#include "renderer/vulkan/Texture2D.hpp"
#include "scene/2d/Sprite2D.hpp"
#include "scene/3d/Camera3D.hpp"
#include "scene/3d/MeshInstance3D.hpp"
#include "scene/3d/Node3D.hpp"
#include "scene/Components.hpp"
#include "QiEngine.hpp"
#include <glm/ext/vector_float2.hpp>
#include <memory>

void SandboxScene::onReady() {

    std::shared_ptr<Qi::Material> greenWallMaterial = Qi::Application::get().getResourceLoader().Create<Qi::Material>(
        Qi::MaterialParameters{
            .albedo = glm::vec3(0.0510f, 0.6510f, 0.0941f),
            .roughness = 0.958f,
            .metallic = 0.0f,
            .emissionColor = glm::vec3(0.0f, 0.0f, 0.0f),
            .emissionPower = 0.0f,
            .specularProbability = 0.06f,
            .isGlass = false,
            .ior = 1.0f,
            .absorption = glm::vec3(0.0f, 0.0f, 0.0f),
            .absorptionStrength = 0.0f
        },
        ""
    );

    std::shared_ptr<Qi::Material> blueWallMaterial = Qi::Application::get().getResourceLoader().Create<Qi::Material>(
        Qi::MaterialParameters{
            .albedo = glm::vec3(0.1490f, 0.3882f, 0.7647f),
            .roughness = 0.967f,
            .metallic = 0.0f,
            .emissionColor = glm::vec3(0.0f, 0.0f, 0.0f),
            .emissionPower = 0.0f,
            .specularProbability = 0.115f,
            .isGlass = false,
            .ior = 1.0f,
            .absorption = glm::vec3(0.0f, 0.0f, 0.0f),
            .absorptionStrength = 0.0f
        },
        ""
    );

    std::shared_ptr<Qi::Material> redWallMaterial = Qi::Application::get().getResourceLoader().Create<Qi::Material>(
        Qi::MaterialParameters{
            .albedo = glm::vec3(0.8000f, 0.1765f, 0.1490f),
            .roughness = 0.985f,
            .metallic = 0.0f,
            .emissionColor = glm::vec3(0.0f, 0.0f, 0.0f), // white
            .emissionPower = 0.0f, // currently zero — no actual light output at this value
            .specularProbability = 0.039f,
            .isGlass = false,
            .ior = 1.0f,
            .absorption = glm::vec3(0.0f, 0.0f, 0.0f),
            .absorptionStrength = 0.0f
        },
        ""
    );

    std::shared_ptr<Qi::Material> lightMaterial = Qi::Application::get().getResourceLoader().Create<Qi::Material>(
        Qi::MaterialParameters{
            .albedo = glm::vec3(1.0f, 1.0f, 1.0f),
            .roughness = 0.0f,
            .metallic = 0.0f,
            .emissionColor = glm::vec3(1.0f, 0.9020f, 0.5333f),
            .emissionPower = 56.0f,
            .specularProbability = 0.0f,
            .isGlass = false,
            .ior = 1.0f,
            .absorption = glm::vec3(0.0f, 0.0f, 0.0f),
            .absorptionStrength = 0.0f
        },
        ""
    );

    std::shared_ptr<Qi::Material> cubeMaterial = Qi::Application::get().getResourceLoader().Create<Qi::Material>(
        Qi::MaterialParameters{

        },
        ""
    );

    m_cube = QiNew<Qi::MeshInstance3D>();
    addNode(m_cube);
    m_cube->setMesh(Qi::MeshPrimitives::BoxMesh());
    m_cube->setScale({7.0f, 1.0f, 16.0f});
    m_cube->setPosition({0.0f, -3.0f, 0.0f});

    m_cube2 = QiNew<Qi::MeshInstance3D>();
    addNode(m_cube2);
    m_cube2->setMesh(Qi::MeshPrimitives::BoxMesh());
    m_cube2->setScale({1.0f, 7.0f, 16.0f});
    m_cube2->setPosition({-2.90f, 0.0f, 0.0f});
    m_cube2->setMaterial(redWallMaterial);

    m_cube3 = QiNew<Qi::MeshInstance3D>();
    addNode(m_cube3);
    m_cube3->setMesh(Qi::MeshPrimitives::BoxMesh());
    m_cube3->setScale({1.0f, 7.0f, 16.0f});
    m_cube3->setPosition({3.0f, 0.0f, 0.0f});
    m_cube3->setMaterial(greenWallMaterial);

    m_cube4 = QiNew<Qi::MeshInstance3D>();
    addNode(m_cube4);
    m_cube4->setMesh(Qi::MeshPrimitives::BoxMesh());
    m_cube4->setScale({7.0f, 1.0f, 16.0f});
    m_cube4->setPosition({0.0f, 3.0f, 0.0f});

    m_cube5 = QiNew<Qi::MeshInstance3D>();
    addNode(m_cube5);
    m_cube5->setMesh(Qi::MeshPrimitives::BoxMesh());
    m_cube5->setScale({7.0f, 7.0f, 1.0f});
    m_cube5->setPosition({0.0f, 0.0f, -3.0f});
    m_cube5->setMaterial(blueWallMaterial);

    m_cube6 = QiNew<Qi::MeshInstance3D>();
    addNode(m_cube6);
    m_cube6->setMesh(Qi::MeshPrimitives::BoxMesh());
    m_cube6->setScale({7.0f, 7.0f, 1.0f});
    m_cube6->setPosition({0.0f, 0.0f, 8.4f});

    displayMesh = QiNew<Qi::MeshInstance3D>();
    addNode(displayMesh);
    displayMesh->setMesh(Qi::MeshPrimitives::BoxMesh());
    displayMesh->setScale({1.5f, 1.5f, 1.5f});
    displayMesh->setPosition({0.0f, -1.766f, 0.0f});
    displayMesh->setRotationEuler({0.0f, 45.0f, 0.0f});
    displayMesh->setMaterial(cubeMaterial);

    lightMesh = QiNew<Qi::MeshInstance3D>();
    addNode(lightMesh);
    lightMesh->setMesh(Qi::MeshPrimitives::BoxMesh());
    lightMesh->setScale({1.0f, 0.1f, 1.0f});
    lightMesh->setPosition({0.0f, 2.539f, 0.0f});
    lightMesh->setMaterial(lightMaterial);

    m_camera = QiNew<CameraController>();
    addNode(m_camera);
    m_camera->setPosition({0.0f, -0.60f, 7.825f});
    m_camera->setFOV(45.0f);

    setActiveCamera(*m_camera);

    // m_directionLight = QiNew<Qi::Node3D>();
    // addNode(m_directionLight);
    // m_directionLight->addComponent<Qi::DirectionalLightComponent>();
}

void SandboxScene::onUpdate(Qi::Timestep ts) {
    Qi::Scene::onUpdate(ts);
    //m_turntableAngle += glm::radians(45.0f) * ts.getSeconds();
    //m_testNode2->setRotation(glm::angleAxis(m_turntableAngle, glm::vec3(0, 1, 0)));
}

void SandboxScene::onEvent(Qi::Event& event) {
    Scene::onEvent(event);
}
