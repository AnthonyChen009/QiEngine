#pragma once

#include "core/Base.hpp"
#include "renderer/IndexBuffer.hpp"
#include "renderer/VertexBuffer.hpp"
#include "renderer/vulkan/VulkanIndexBuffer.hpp"
#include "renderer/vulkan/VulkanVertexBuffer.hpp"
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

private:
    Ref<VertexBuffer> m_vertexBuffer;
    Ref<IndexBuffer> m_indexBuffer;
};

}
