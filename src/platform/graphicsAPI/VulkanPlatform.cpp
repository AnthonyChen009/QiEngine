#include "VulkanPlatform.hpp"

#include "SDL3/SDL_vulkan.h"
#include "core/Assert.hpp"
#include <SDL3/SDL.h>

namespace Qi::VulkanPlatform {

const char* const* getRequiredVulkanExtensions(uint32_t& count) {
    return SDL_Vulkan_GetInstanceExtensions(&count);
}

VkSurfaceKHR createVulkanSurface(VkInstance instance, Window& window) {
    VkSurfaceKHR surface = VK_NULL_HANDLE;

    SDL_Window* nativeWindow = static_cast<SDL_Window*>(window.getNativeWindow());
    bool success = SDL_Vulkan_CreateSurface(nativeWindow, instance, nullptr, &surface);

    QI_RENDERER_ASSERT(success, "Failed to create Vulkan surface!");

    return surface;
}

void getFrameBufferSize(Window& window, int& width, int& height) {
    SDL_Window* nativeWindow = static_cast<SDL_Window*>(window.getNativeWindow());
    SDL_GetWindowSizeInPixels(nativeWindow, &width, &height);
}

}
