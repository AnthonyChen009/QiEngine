#include "scene/2d/Sprite2D.hpp"
#include "core/Log.hpp"
#include "renderer/vulkan/Texture2D.hpp"
#include <memory>
namespace Qi {

Sprite2D::Sprite2D(const std::string& name, glm::vec4 color, glm::vec2 size) : Node(name), m_color(color), m_size(size) {}

void Sprite2D::onReady() {
    addComponent<TransformComponent>();
    addComponent<SpriteComponent>(m_color);
    auto& transform = getComponent<TransformComponent>();
    transform.size  = m_size;
}

void Sprite2D::setColor(const glm::vec4& color) {
    m_color = color;
    getComponent<SpriteComponent>().color = color;
}

void Sprite2D::setSize(const glm::vec2& size) {
    m_size = size;
    getComponent<TransformComponent>().size = size;
}

void Sprite2D::setPosition(const glm::vec2& pos) {
    getComponent<TransformComponent>().position = pos;
}

const glm::vec2& Sprite2D::getPosition() const {
    return getComponent<TransformComponent>().position;
}

const glm::vec2& Sprite2D::getWorldPosition() const {
    return getComponent<TransformComponent>().worldPosition;
}

void Sprite2D::setTexture(const std::shared_ptr<Texture2D>& texture) {
    //QI_CORE_INFO("Setting texture: {0}", path);
    getComponent<SpriteComponent>().texture = texture;
}

}
