#pragma once
#include <glm/glm.hpp>

namespace Qi {
struct RTCameraUBO {
    glm::mat4 invView;
    glm::mat4 invProj;
};
}
