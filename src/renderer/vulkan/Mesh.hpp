#pragma once

#include "core/Base.hpp"
#include "renderer/IndexBuffer.hpp"
#include "renderer/VertexBuffer.hpp"
#include "renderer/vulkan/VulkanIndexBuffer.hpp"
#include "renderer/vulkan/VulkanVertexBuffer.hpp"
#include "renderer/vulkan/VulkanAccelerationStructure.hpp"
#include <memory>

namespace Qi {

class Mesh {
public:
    Mesh(Ref<VertexBuffer> vertexBuffer, Ref<IndexBuffer> indexBuffer) : m_vertexBuffer(vertexBuffer), m_indexBuffer(indexBuffer) {

    }

    const VertexBuffer& getVertexBuffer() const {
        return *m_vertexBuffer;
    }

    const IndexBuffer& getIndexBuffer() const {
        return *m_indexBuffer;
    }

    uint32_t getIndexCount() const {
        return m_indexBuffer->getCount();
    }

    void setBLAS(std::unique_ptr<VulkanAccelerationStructure> blas) { m_blas = std::move(blas); }
    const VulkanAccelerationStructure* getBLAS() const { return m_blas.get(); }
    bool hasBLAS() const { return m_blas != nullptr; }

private:
    Ref<VertexBuffer> m_vertexBuffer;
    Ref<IndexBuffer> m_indexBuffer;
    std::unique_ptr<VulkanAccelerationStructure> m_blas;
};

}
