#pragma once
#include "scene/Node.hpp"
#include "scene/Components.hpp"
#include <glm/glm.hpp>

namespace Qi {

class Sprite2D : public Node {

public:
    Sprite2D(const std::string& name = "Sprite2D", glm::vec4 color = { 1.0f, 1.0f, 1.0f, 1.0f }, glm::vec2 size  = { 50.0f, 50.0f });

    void onReady() override;

    void setColor(const glm::vec4& color);
    void setSize(const glm::vec2& size);
    void setPosition(const glm::vec2& pos);

    const glm::vec4& getColor() const { return m_color; }
    const glm::vec2& getSize() const { return m_size; }
    const glm::vec2& getPosition() const;
    const glm::vec2& getWorldPosition() const;
    void setTexture(const std::string& path);

private:
    glm::vec4 m_color;
    glm::vec2 m_size;
};

}
