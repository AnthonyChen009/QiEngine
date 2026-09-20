#pragma once
#include "renderer/Material.hpp"
#include <cstdint>

namespace Qi {

class VulkanMaterial : public Material {

public:
    VulkanMaterial(const MaterialParameters& params, const std::string& path = ""): m_params(params), m_path(path) {}
    const std::string& getPath() const override { return m_path; }
    uint32_t getIndex() const override { return m_index; }
    const MaterialParameters& getParameters() const override { return m_params; }
    void setIndex(uint32_t ind) override { m_index = ind; }
private:
    MaterialParameters m_params;
    std::string m_path;
    uint32_t m_index = 0;

};

}
