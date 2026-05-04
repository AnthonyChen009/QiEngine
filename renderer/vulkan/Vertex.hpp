#pragma once
#include <array>
#include <cstddef>
#include <vector>

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace Qi {

struct Vertex {
    glm::vec2 pos;
    glm::vec3 color;

    static VkVertexInputBindingDescription getBindingDescription() {
        VkVertexInputBindingDescription binding{};
        binding.binding = 0;
        binding.stride = sizeof(Vertex);
        binding.inputRate = VK_VERTEX_INPUT_RATE_VERTEX;
        return binding;
    }

    static std::array<VkVertexInputAttributeDescription, 2> getAttributeDescriptions() {
        std::array<VkVertexInputAttributeDescription, 2> attributes{};

        attributes[0].binding = 0;
        attributes[0].location = 0;
        attributes[0].format = VK_FORMAT_R32G32_SFLOAT;
        attributes[0].offset = offsetof(Vertex, pos);

        attributes[1].binding = 0;
        attributes[1].location = 1;
        attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributes[1].offset = offsetof(Vertex, color);

        return attributes;
    }
};

inline const std::vector<Vertex> vertices = {
    // first triangle
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}, // bottom left
    {{ 0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}}, // bottom right
    {{ 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}}, // top right

    // second triangle
    {{ 0.5f,  0.5f}, {0.0f, 0.0f, 1.0f}}, // top right
    {{-0.5f,  0.5f}, {1.0f, 1.0f, 0.0f}}, // top left
    {{-0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}}  // bottom left
};

}
