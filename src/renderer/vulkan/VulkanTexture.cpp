#include "VulkanTexture.hpp"
#include "VulkanBuffer.hpp"
#include "VulkanImage.hpp"
#include "VulkanCommands.hpp"
#include "core/Assert.hpp"
#include "renderer/vulkan/VulkanSampler.hpp"
#include <external/stb/stb_image.h>

namespace Qi {

VulkanTexture::VulkanTexture(
    VkDevice device,
    VkPhysicalDevice physicalDevice,
    VkCommandPool commandPool,
    VkQueue graphicsQueue,
    const std::string& path
) : m_device(device), m_physicalDevice(physicalDevice), m_commandPool(commandPool), m_graphicsQueue(graphicsQueue), m_imgPath(path) {
    //QI_CORE_INFO("Loading texture from: {0}", path);
   // QI_CORE_INFO("Current path: {0}", std::filesystem::current_path().string());
    int texWidth, texHeight, texChannels;
    stbi_set_flip_vertically_on_load(true);
    stbi_uc* pixels = stbi_load(path.c_str(), &texWidth, &texHeight, &texChannels, STBI_rgb_alpha);
    m_width = (uint32_t)texWidth;
    m_height = (uint32_t)texHeight;
    VkDeviceSize imageSize = m_width * m_height * 4;
    QI_RENDERER_ASSERT(pixels, "Failed to load texture image!");

    VulkanBuffer stagingBuffer(m_device, physicalDevice);
    stagingBuffer.create(imageSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);

    void* data;
    vkMapMemory(m_device, stagingBuffer.getMemory(), 0, imageSize, 0, &data);
    memcpy(data, pixels, static_cast<size_t>(imageSize));
    vkUnmapMemory(m_device, stagingBuffer.getMemory());

    stbi_image_free(pixels);

    VulkanImage::createImage(
        m_device,
        physicalDevice,
        m_width,
        m_height,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_TRANSFER_DST_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_image,
        m_imageMemory
    );

    VulkanCommands::transitionImageLayout(m_device, m_commandPool,  m_graphicsQueue, m_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL);
    VulkanCommands::copyBufferToImage(m_device, m_commandPool,  m_graphicsQueue, stagingBuffer.getBuffer(), m_image, static_cast<uint32_t>(texWidth), static_cast<uint32_t>(texHeight));
    VulkanCommands::transitionImageLayout(m_device, m_commandPool,  m_graphicsQueue, m_image, VK_FORMAT_R8G8B8A8_SRGB, VK_IMAGE_LAYOUT_TRANSFER_DST_OPTIMAL, VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL);

    m_imageView = VulkanImage::createImageView(
        m_device,
        m_image,
        VK_FORMAT_R8G8B8A8_SRGB,
        VK_IMAGE_ASPECT_COLOR_BIT
    );

    m_sampler = VulkanSampler::create(m_device, m_physicalDevice);
}

VkImageView VulkanTexture::getImageView() const {
    return m_imageView;
}

VkSampler VulkanTexture::getSampler() const {
    return m_sampler;
}

uint32_t VulkanTexture::getWidth() const {
    return m_width;
}

uint32_t VulkanTexture::getHeight() const {
    return m_height;
}

VulkanTexture::~VulkanTexture() {
    QI_INFO("Texture being destroyed");
    if (m_sampler) vkDestroySampler(m_device, m_sampler, nullptr);
    if (m_imageView) vkDestroyImageView(m_device, m_imageView, nullptr);
    if (m_image) vkDestroyImage(m_device, m_image, nullptr);
    if (m_imageMemory) vkFreeMemory(m_device, m_imageMemory, nullptr);
}

}
