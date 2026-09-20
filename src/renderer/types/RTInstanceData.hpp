#pragma once
#include <vulkan/vulkan.h>
#include <glm/glm.hpp>

namespace Qi {

struct RTInstanceData {
    VkDeviceAddress blasAddress;
    glm::mat4 transform;
    uint32_t instanceCustomIndex = 0;
    uint8_t mask = 0xFF;
    VkDeviceAddress vertexBufferAddress;
    VkDeviceAddress indexBufferAddress;
    uint32_t materialIndex = 0;
};

struct InstanceAddresses {
    VkDeviceAddress vertexBufferAddress;
    VkDeviceAddress indexBufferAddress;
    uint32_t materialIndex;
};

}
