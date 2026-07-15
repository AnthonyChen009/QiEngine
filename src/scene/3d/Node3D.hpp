#pragma once
#include "scene/Components.hpp"
#include "scene/Node.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>

namespace Qi {

class Node3D : public Node {

public:
    void onReady() override;
    void onUpdate(Timestep ts) override;
    void onEvent(Event& event) override;

    void setPosition(const glm::vec3& position);
    const glm::vec3& getPosition() const;

    void setRotation(const glm::quat& rotation);
    const glm::quat& getRotation() const;

    void setRotationEuler(const glm::vec3& rotation);
    glm::vec3 getRotationEuler() const;

    void setScale(const glm::vec3& scale);
    const glm::vec3& getScale() const;

    glm::mat4 getTransform() const;
    glm::mat4 getWorldTransform() const;

private:

};

}
