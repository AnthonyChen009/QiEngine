#pragma once
#include <array>
#include <cstddef>
#include <vector>

#include <glm/glm.hpp>
#include <vulkan/vulkan.h>

namespace Qi {

struct Vertex {
    glm::vec3 pos;
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
        attributes[0].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributes[0].offset = offsetof(Vertex, pos);

        attributes[1].binding = 0;
        attributes[1].location = 1;
        attributes[1].format = VK_FORMAT_R32G32B32_SFLOAT;
        attributes[1].offset = offsetof(Vertex, color);

        return attributes;
    }
};

// inline const std::vector<Vertex> vertices = {
//     {{-0.5f, -0.5f, -0.5f}, {1.0f, 0.0f, 0.0f}},
//     {{ 0.5f, -0.5f, -0.5f}, {0.0f, 1.0f, 0.0f}},
//     {{ 0.5f,  0.5f, -0.5f}, {0.0f, 0.0f, 1.0f}},
//     {{-0.5f,  0.5f, -0.5f}, {1.0f, 1.0f, 0.0f}},

//     {{-0.5f, -0.5f,  0.5f}, {1.0f, 0.0f, 1.0f}},
//     {{ 0.5f, -0.5f,  0.5f}, {0.0f, 1.0f, 1.0f}},
//     {{ 0.5f,  0.5f,  0.5f}, {1.0f, 1.0f, 1.0f}},
//     {{-0.5f,  0.5f,  0.5f}, {0.3f, 0.3f, 0.3f}},
// };

inline const std::vector<Vertex> vertices = {
    {{-0.5f, -0.5f, 0.0f}, {1.0f, 0.0f, 0.0f}}, // left  - red
    {{ 0.5f, -0.5f, 0.0f}, {0.0f, 1.0f, 0.0f}}, // right - green
    {{ 0.0f,  0.5f, 0.0f}, {0.0f, 0.0f, 1.0f}}, // top   - blue
};

inline const std::vector<uint32_t> indices = {
    0, 2, 1, 2, 0, 3, // back
    4, 5, 6, 6, 7, 4, // front
    0, 4, 7, 7, 3, 0, // left
    1, 2, 6, 6, 5, 1, // right
    3, 7, 6, 6, 2, 3, // top
    0, 1, 5, 5, 4, 0  // bottom
};

}
