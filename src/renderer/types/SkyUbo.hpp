#pragma once

#include <glm/glm.hpp>

namespace Qi {

struct alignas(16) SkyUniformBufferObject {
    alignas(16) glm::mat4 invViewProj;
};

}
