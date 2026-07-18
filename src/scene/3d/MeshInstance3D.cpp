#include "MeshInstance3D.hpp"
#include "scene/3d/Node3D.hpp"
#include "scene/Components.hpp"
#include "events/Event.hpp"

namespace Qi {

MeshInstance3D::MeshInstance3D(const std::string& name) : Node3D(name) {

}

void MeshInstance3D::onReady() {
    Node3D::onReady();
    addComponent<MeshComponent>();
}

void MeshInstance3D::onUpdate(Timestep ts) {

}

void MeshInstance3D::onEvent(Event& event) {

}

void MeshInstance3D::setMesh(const std::shared_ptr<Mesh>& mesh) {
    getComponent<MeshComponent>().mesh = mesh;
}

const Ref<Mesh>& MeshInstance3D::getMesh() const {
    return getComponent<MeshComponent>().mesh;
}

}
