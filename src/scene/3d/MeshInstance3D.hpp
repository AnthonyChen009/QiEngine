#pragma once

#include "renderer/Material.hpp"
#include "renderer/vulkan/Mesh.hpp"
#include "scene/3d/Node3D.hpp"
#include <memory>
namespace Qi {

class MeshInstance3D : public Node3D {

public:
    MeshInstance3D(const std::string& name = "MeshInstance3D");
    void onReady() override;
    void onUpdate(Timestep ts) override;
    void onEvent(Event& event) override;

    void setMesh(const std::shared_ptr<Mesh>& mesh);
    void setMaterial(std::shared_ptr<Material>& mat);
    const Ref<Mesh>& getMesh() const;

private:

};

}
