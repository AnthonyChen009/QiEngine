#pragma once
#include <vector>

namespace Qi {

static constexpr bool enableValidationLayers =
#ifdef QI_DEBUG
    true;
#else
    false;
#endif

static const std::vector<const char*> validationLayers = {
    "VK_LAYER_KHRONOS_validation"
};

}
