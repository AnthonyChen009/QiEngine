#pragma once

#include <optional>
#include "cstdint"
#include "vulkan/vulkan.h"

namespace Qi {
struct QueueFamilyIndices {
    std::optional<uint32_t> graphicsFamily;
    std::optional<uint32_t> presentFamily;

    bool isComplete() {
        return graphicsFamily.has_value();
    }
};

}
