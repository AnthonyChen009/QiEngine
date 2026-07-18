#include "Node3D.hpp"
#include "scene/Components.hpp"
#include "scene/Node.hpp"
#include <glm/fwd.hpp>
#include <glm/gtc/quaternion.hpp>


namespace Qi {

Node3D::Node3D(const std::string& name) : Node(name) {

}

void Node3D::onReady() {
    addComponent<Transform3DComponent>();
}

void Node3D::onUpdate(Timestep ts) {

}

void Node3D::onEvent(Event& event) {

}

void Node3D::setPosition(const glm::vec3& position) {
    getComponent<Transform3DComponent>().position = position;
}

const glm::vec3& Node3D::getPosition() const {
    return getComponent<Transform3DComponent>().position;
}

void Node3D::setRotation(const glm::quat& rotation) {
    getComponent<Transform3DComponent>().rotation = rotation;
}

const glm::quat& Node3D::getRotation() const {
    return getComponent<Transform3DComponent>().rotation;
}

void Node3D::setRotationEuler(const glm::vec3& rotation) {
    getComponent<Transform3DComponent>().rotation = glm::quat(glm::radians(rotation));
}

glm::vec3 Node3D::getRotationEuler() const {
    return glm::degrees(glm::eulerAngles(getComponent<Transform3DComponent>().rotation));
}

void Node3D::setScale(const glm::vec3& scale) {
    getComponent<Transform3DComponent>().scale = scale;
}

const glm::vec3& Node3D::getScale() const {
    return getComponent<Transform3DComponent>().scale;
}

glm::mat4 Node3D::getTransform() const {
    return getComponent<Transform3DComponent>().localTransform;
}

glm::mat4 Node3D::getWorldTransform() const {
    return getComponent<Transform3DComponent>().worldTransform;
}

}
