#include "SandboxScene.hpp"
#include "QiEngine.hpp"
#include "os/Memory.hpp"
#include "scene/2d/Sprite2D.hpp"

void SandboxScene::onReady() {
    for (int i = 0; i < 2000; i++) {
        auto* sprite = QiNew<Qi::Sprite2D>("Sprite_" + std::to_string(i));
        addNode(sprite, 0);
        sprite->setTexture("images/textureTest.jpg");
        sprite->setSize(glm::vec2(100, 100));

        // spread them out randomly
        float x = ((float)rand() / RAND_MAX) * 1280.0f - 640.0f;
        float y = ((float)rand() / RAND_MAX) * 720.0f - 360.0f;
        sprite->setPosition(glm::vec2(x, y));

        sprite->addComponent<Qi::RigidbodyComponent>(glm::vec2{
            ((float)rand() / RAND_MAX) * 400.0f - 200.0f,
            ((float)rand() / RAND_MAX) * 400.0f - 200.0f
        });
    }
}

void SandboxScene::onUpdate(Qi::Timestep ts) {
    float halfW = 1280.0f / 2.0f;
    float halfH = 720.0f / 2.0f;

    auto view = getRegistry().view<Qi::TransformComponent, Qi::RigidbodyComponent>();
    for (auto entity : view) {
        auto& transform = view.get<Qi::TransformComponent>(entity);
        auto& rb = view.get<Qi::RigidbodyComponent>(entity);

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
}

void SandboxScene::onEvent(Qi::Event& e) {
    Scene::onEvent(e);
}
