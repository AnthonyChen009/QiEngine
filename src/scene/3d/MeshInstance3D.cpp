#include "MeshInstance3D.hpp"
#include "scene/Components.hpp"
#include "events/Event.hpp"

namespace Qi {

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
