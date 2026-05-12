#include "renderer/vulkan/Texture2D.hpp"
#include "renderer/vulkan/VulkanImage.hpp"
#define GLM_FORCE_DEPTH_ZERO_TO_ONE
#include "renderer/vulkan/VulkanRenderer.hpp"
#include <cstdint>
#include <vector>
#include <vulkan/vulkan.h>
#include "core/FileSystem.hpp"
#include "core/Assert.hpp"
#include "renderer/types/Vertex.hpp"
#include <glm/glm.hpp>
#include "VulkanCommands.hpp"
#include "renderer/types/UniformBufferObject.hpp"
#include "types/PushConstants.hpp"

namespace Qi {

void VulkanRenderer::init(Window& window) {
    m_window = &window;
    createInstance(window.getWindowName());
    m_surface.emplace(m_instance.getVkInstance(), window);
    m_vulkanDevice.emplace(m_instance.getVkInstance(), m_surface->getVkSurface());
    m_swapChain.emplace(m_vulkanDevice->getDevice(), m_vulkanDevice->getPhysicalDevice(), m_surface->getVkSurface(), window, m_vulkanDevice->findQueueFamilies(m_vulkanDevice->getPhysicalDevice()));
    m_renderPass.emplace(m_vulkanDevice->getDevice(), m_vulkanDevice->getPhysicalDevice(), m_swapChain->getImageFormat());
    m_graphicsPipeline.emplace(m_vulkanDevice->getDevice(), m_renderPass->getRenderPass());
    createCommandPool();
    createDepthResources();
    createFrameBuffers();
    m_vertexBuffer = std::make_unique<VulkanVertexBuffer>(m_vulkanDevice->getDevice(), m_vulkanDevice->getPhysicalDevice(), m_commandPool, m_vulkanDevice->getPresentQueue(), vertices);
    m_indexBuffer = std::make_unique<VulkanIndexBuffer>(m_vulkanDevice->getDevice(), m_vulkanDevice->getPhysicalDevice(), m_commandPool, m_vulkanDevice->getPresentQueue(), indices);

    createUniformBuffers();
    createDescriptorPool();
    createCommandBuffers();
    createSyncObjects();
    createDescriptorSets();
    //log
    VkPhysicalDeviceProperties properties{};
    vkGetPhysicalDeviceProperties(m_vulkanDevice->getPhysicalDevice(), &properties);

    QI_CORE_INFO("Vulkan initialized");
    QI_CORE_INFO("GPU: {0}", properties.deviceName);
    QI_CORE_INFO("Vulkan API Version: {0}.{1}.{2}",
        VK_VERSION_MAJOR(properties.apiVersion),
        VK_VERSION_MINOR(properties.apiVersion),
        VK_VERSION_PATCH(properties.apiVersion));

    QI_CORE_INFO("Driver Version: {0}", properties.driverVersion);
}

bool VulkanRenderer::beginFrame() {
    m_pipelineBound = false;

    vkWaitForFences(
        m_vulkanDevice->getDevice(),
        1,
        &m_inFlightFences[m_currentFrame],
        VK_TRUE,
        UINT64_MAX
    );
    uint32_t imageIndex;

    VkResult acquireResult = vkAcquireNextImageKHR(m_vulkanDevice->getDevice(), m_swapChain->getSwapChain(), UINT64_MAX, m_imageAvailableSemaphores[m_currentFrame], VK_NULL_HANDLE, &imageIndex);

    if (acquireResult == VK_ERROR_OUT_OF_DATE_KHR) {
        recreateSwapChain();
        return false;
    } else if (acquireResult != VK_SUCCESS && acquireResult != VK_SUBOPTIMAL_KHR) {
        QI_RENDERER_ASSERT(false, "Failed to acquire swap chain image!");
        return false;
    }

    vkResetFences(m_vulkanDevice->getDevice(), 1, &m_inFlightFences[m_currentFrame]);

    vkResetCommandBuffer(m_commandBuffers[m_currentFrame], 0);

    m_currentImageIndex = imageIndex;

    beginCommandBuffer(m_commandBuffers[m_currentFrame], m_currentImageIndex);

    return true;
}

void VulkanRenderer::endFrame() {
    VkCommandBuffer commandBuffer = m_commandBuffers[m_currentFrame];

    vkCmdEndRenderPass(commandBuffer);

    VkResult endResult = vkEndCommandBuffer(commandBuffer);
    QI_RENDERER_ASSERT(endResult == VK_SUCCESS, "Failed to record command buffer!");

    VkSemaphore waitSemaphores[] = {
        m_imageAvailableSemaphores[m_currentFrame]
    };

    VkPipelineStageFlags waitStages[] = {
        VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT
    };

    VkSemaphore signalSemaphores[] = {
        m_renderFinishedSemaphores[m_currentFrame]
    };

    VkSubmitInfo submitInfo{};
    submitInfo.sType = VK_STRUCTURE_TYPE_SUBMIT_INFO;
    submitInfo.waitSemaphoreCount = 1;
    submitInfo.pWaitSemaphores = waitSemaphores;
    submitInfo.pWaitDstStageMask = waitStages;
    submitInfo.commandBufferCount = 1;
    submitInfo.pCommandBuffers = &commandBuffer;
    submitInfo.signalSemaphoreCount = 1;
    submitInfo.pSignalSemaphores = signalSemaphores;
    VkResult result = vkQueueSubmit(m_vulkanDevice->getGraphicsQueue(), 1, &submitInfo, m_inFlightFences[m_currentFrame]);
    QI_RENDERER_ASSERT(result == VK_SUCCESS, "Failed to submit draw command buffer!");

    VkSwapchainKHR swapChains[] = { m_swapChain->getSwapChain() };

    VkPresentInfoKHR presentInfo{};
    presentInfo.sType = VK_STRUCTURE_TYPE_PRESENT_INFO_KHR;
    presentInfo.waitSemaphoreCount = 1;
    presentInfo.pWaitSemaphores = signalSemaphores;
    presentInfo.swapchainCount = 1;
    presentInfo.pSwapchains = swapChains;
    presentInfo.pImageIndices = &m_currentImageIndex;

    VkResult presentResult = vkQueuePresentKHR(m_vulkanDevice->getPresentQueue(), &presentInfo);

    if (presentResult == VK_ERROR_OUT_OF_DATE_KHR ||
        presentResult == VK_SUBOPTIMAL_KHR ||
        m_frameBufferResized) {
        m_frameBufferResized = false;
        recreateSwapChain();
    } else if (presentResult != VK_SUCCESS) {
        QI_RENDERER_ASSERT(false, "Failed to present swap chain image!");
    }

    m_currentFrame = (m_currentFrame + 1) % MAX_FRAMES_IN_FLIGHT;
}

void VulkanRenderer::onWindowResize(uint32_t width, uint32_t height) {
    m_winHeight = height;
    m_winWidth = width;
    m_frameBufferResized = true;

}

void VulkanRenderer::drawIndexed(uint32_t count)  {
    QI_RENDERER_ASSERT(m_pipelineBound, "Cannot draw before binding a graphics pipeline!");
    QI_RENDERER_ASSERT(m_indexBuffer, "Index buffer has not been created!");

    vkCmdDrawIndexed(m_commandBuffers[m_currentFrame], m_indexBuffer->getCount(), 1, 0, 0, 0);
}

void VulkanRenderer::createInstance(const std::string& appName) {
    m_instance.createVkInstance(appName);
}

void VulkanRenderer::createFrameBuffers() {
    const std::vector<VkImageView>& imgViews = m_swapChain->getImageViews();
    m_swapChainFrameBuffers.resize(imgViews.size());

    for (size_t i = 0; i < imgViews.size(); i++) {
        std::array<VkImageView, 2> attachments = { imgViews[i], m_depthImageView };

        VkFramebufferCreateInfo framebufferInfo{};
        framebufferInfo.sType = VK_STRUCTURE_TYPE_FRAMEBUFFER_CREATE_INFO;
        framebufferInfo.renderPass = m_renderPass->getRenderPass();
        framebufferInfo.attachmentCount = static_cast<uint32_t>(attachments.size());
        framebufferInfo.pAttachments = attachments.data();
        framebufferInfo.width = m_swapChain->getExtent().width;
        framebufferInfo.height = m_swapChain->getExtent().height;
        framebufferInfo.layers = 1;

        VkResult result = vkCreateFramebuffer(m_vulkanDevice->getDevice(), &framebufferInfo, nullptr, &m_swapChainFrameBuffers[i]);
        QI_RENDERER_ASSERT(result == VK_SUCCESS, "Failed to create framebuffer!");
    }
}

void VulkanRenderer::createCommandPool() {
    QueueFamilyIndices queueFamilyIndices = m_vulkanDevice->findQueueFamilies(m_vulkanDevice->getPhysicalDevice());
    VkCommandPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_COMMAND_POOL_CREATE_INFO;
    poolInfo.flags = VK_COMMAND_POOL_CREATE_RESET_COMMAND_BUFFER_BIT;
    poolInfo.queueFamilyIndex = queueFamilyIndices.graphicsFamily.value();

    VkResult result = vkCreateCommandPool(m_vulkanDevice->getDevice(), &poolInfo, nullptr, &m_commandPool);
    QI_RENDERER_ASSERT(result == VK_SUCCESS, "Failed to create command pool!");
}

void VulkanRenderer::createCommandBuffers() {
    m_commandBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    VkCommandBufferAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_ALLOCATE_INFO;
    allocInfo.commandPool = m_commandPool;
    allocInfo.level = VK_COMMAND_BUFFER_LEVEL_PRIMARY;
    allocInfo.commandBufferCount = static_cast<uint32_t>(m_commandBuffers.size());

    VkResult result = vkAllocateCommandBuffers(m_vulkanDevice->getDevice(), &allocInfo, m_commandBuffers.data());
    QI_RENDERER_ASSERT(result == VK_SUCCESS, "Failed to allocate command buffers!");
}

void VulkanRenderer::beginCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;
    VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    QI_RENDERER_ASSERT(result == VK_SUCCESS, "Failed to begin recording command buffer!");

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_renderPass->getRenderPass();
    renderPassInfo.framebuffer = m_swapChainFrameBuffers[imageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_swapChain->getExtent();

    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.0f, 0.0f, 0.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};

    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();

    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);
    //vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline);

    VkViewport viewport{};
    viewport.x = 0.0f;
    viewport.y = 0.0f;
    viewport.width = static_cast<float>(m_swapChain->getExtent().width);
    viewport.height = static_cast<float>(m_swapChain->getExtent().height);
    viewport.minDepth = 0.0f;
    viewport.maxDepth = 1.0f;
    vkCmdSetViewport(commandBuffer, 0, 1, &viewport);

    VkRect2D scissor{};
    scissor.offset = {0, 0};
    scissor.extent = m_swapChain->getExtent();
    vkCmdSetScissor(commandBuffer, 0, 1, &scissor);
}

void VulkanRenderer::createSyncObjects() {
    m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_renderFinishedSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkResult imageSemaphoreResult = vkCreateSemaphore(m_vulkanDevice->getDevice(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]);
        VkResult renderSemaphoreResult = vkCreateSemaphore(m_vulkanDevice->getDevice(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]);
        VkResult fenceResult = vkCreateFence(m_vulkanDevice->getDevice(), &fenceInfo, nullptr, &m_inFlightFences[i]);
        QI_RENDERER_ASSERT(imageSemaphoreResult == VK_SUCCESS && renderSemaphoreResult == VK_SUCCESS && fenceResult == VK_SUCCESS, "Failed to create synchronization objects!");
    }

}

void VulkanRenderer::bindPipeline() {
    QI_RENDERER_ASSERT(m_vertexBuffer, "Vertex buffer has not been created!");
    vkCmdBindPipeline(m_commandBuffers[m_currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline->getPipeline());

    m_vertexBuffer->bind(m_commandBuffers[m_currentFrame]);
    m_indexBuffer->bind(m_commandBuffers[m_currentFrame]);

    vkCmdBindDescriptorSets(m_commandBuffers[m_currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, m_graphicsPipeline->getPipelineLayout(), 0, 1, &m_descriptorSets[m_currentFrame], 0, nullptr);

    m_pipelineBound = true;
}

void VulkanRenderer::cleanupSwapChain() {
    for (auto framebuffer : m_swapChainFrameBuffers) {
        vkDestroyFramebuffer(m_vulkanDevice->getDevice(), framebuffer, nullptr);
    }
    m_swapChainFrameBuffers.clear();

    if (m_depthImageView != VK_NULL_HANDLE) {
        vkDestroyImageView(m_vulkanDevice->getDevice(), m_depthImageView, nullptr);
        m_depthImageView = VK_NULL_HANDLE;
    }

    if (m_depthImage != VK_NULL_HANDLE) {
        vkDestroyImage(m_vulkanDevice->getDevice(), m_depthImage, nullptr);
        m_depthImage = VK_NULL_HANDLE;
    }

    if (m_depthImageMemory != VK_NULL_HANDLE) {
        vkFreeMemory(m_vulkanDevice->getDevice(), m_depthImageMemory, nullptr);
        m_depthImageMemory = VK_NULL_HANDLE;
    }
}

void VulkanRenderer::recreateSwapChain() {

    m_window->waitForValidFramebufferSize();

    vkDeviceWaitIdle(m_vulkanDevice->getDevice());

    cleanupSwapChain();

    m_swapChain->recreate(*m_window);
    createDepthResources();
    createFrameBuffers();
}

void VulkanRenderer::createUniformBuffers() {
    m_uniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        m_uniformBuffers[i] = std::make_unique<VulkanUniformBuffer>(m_vulkanDevice->getDevice(), m_vulkanDevice->getPhysicalDevice(), sizeof(UniformBufferObject));
    }
}

void VulkanRenderer::updateUniformBuffer() {
    UniformBufferObject ubo{};
    ubo.model = glm::mat4(1.0f);
    ubo.view = glm::mat4(1.0f);
    float width = static_cast<float>(m_swapChain->getExtent().width);
    float height = static_cast<float>(m_swapChain->getExtent().height);
    ubo.proj = glm::ortho(-width/2.0f, width/2.0f, -height/2.0f, height/2.0f, -1.0f, 1.0f);
    ubo.proj[1][1] *= -1;
    m_uniformBuffers[m_currentFrame]->setData(&ubo, sizeof(ubo));
}

void VulkanRenderer::createDescriptorPool() {
    std::array<VkDescriptorPoolSize, 2> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = 1024 * static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);

    VkResult result = vkCreateDescriptorPool(m_vulkanDevice->getDevice(), &poolInfo, nullptr, &m_descriptorPool);
    QI_RENDERER_ASSERT(result == VK_SUCCESS, "Failed to create descriptor pool!");
}

void VulkanRenderer::createDescriptorSets() {
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_graphicsPipeline->getDescriptorSetLayout());

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    m_descriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    VkResult result = vkAllocateDescriptorSets(m_vulkanDevice->getDevice(), &allocInfo, m_descriptorSets.data());
    QI_RENDERER_ASSERT(result == VK_SUCCESS, "Failed to allocate descriptor sets!");

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = m_uniformBuffers[i]->getBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = sizeof(UniformBufferObject);

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = m_descriptorSets[i];
        write.dstBinding = 0;
        write.dstArrayElement = 0;
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.descriptorCount = 1;
        write.pBufferInfo = &bufferInfo;

        vkUpdateDescriptorSets(m_vulkanDevice->getDevice(), 1, &write, 0, nullptr);
    }
}


void VulkanRenderer::createDepthResources() {
    VkFormat depthFormat = VulkanUtils::findDepthFormat(m_vulkanDevice->getPhysicalDevice());
    VulkanImage::createImage(m_vulkanDevice->getDevice(), m_vulkanDevice->getPhysicalDevice(), m_swapChain->getExtent().width, m_swapChain->getExtent().height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_depthImage, m_depthImageMemory);
    m_depthImageView = VulkanImage::createImageView(m_vulkanDevice->getDevice(), m_depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    VulkanCommands::transitionImageLayout(m_vulkanDevice->getDevice(), m_commandPool, m_vulkanDevice->getPresentQueue(), m_depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

bool VulkanRenderer::hasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void VulkanRenderer::pushConstants(const PushConstant2D& push) {
    vkCmdPushConstants(
        m_commandBuffers[m_currentFrame],
        m_graphicsPipeline->getPipelineLayout(),
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        0,
        sizeof(PushConstant2D),
        &push
    );
}

Texture2D* VulkanRenderer::getOrLoadTexture(const std::string& path) {

    auto it = m_textureCache.find(path);
    if (it != m_textureCache.end())
        return it->second.get();

    auto texture = std::make_unique<VulkanTexture>(
        m_vulkanDevice->getDevice(),
        m_vulkanDevice->getPhysicalDevice(),
        m_commandPool,
        m_vulkanDevice->getPresentQueue(),
        path
    );

    uint32_t index = m_nextTextureIndex++;
    texture->setIndex(index);
    // write into descriptor array for all frames
    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorImageInfo imageInfo{};
        imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        imageInfo.imageView = texture->getImageView();
        imageInfo.sampler = texture->getSampler();

        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = m_descriptorSets[i];
        write.dstBinding = 1;
        write.dstArrayElement = index;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.descriptorCount = 1;
        write.pImageInfo = &imageInfo;

        vkUpdateDescriptorSets(m_vulkanDevice->getDevice(), 1, &write, 0, nullptr);
    }
    Texture2D* ptr = texture.get();
    m_textureCache[path] = std::move(texture);
    return ptr;
}

void VulkanRenderer::shutdown() {
    if (m_vulkanDevice->getDevice() != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(m_vulkanDevice->getDevice());
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (m_renderFinishedSemaphores[i] != VK_NULL_HANDLE)
            vkDestroySemaphore(m_vulkanDevice->getDevice(), m_renderFinishedSemaphores[i], nullptr);

        if (m_imageAvailableSemaphores[i] != VK_NULL_HANDLE)
            vkDestroySemaphore(m_vulkanDevice->getDevice(), m_imageAvailableSemaphores[i], nullptr);

        if (m_inFlightFences[i] != VK_NULL_HANDLE)
            vkDestroyFence(m_vulkanDevice->getDevice(), m_inFlightFences[i], nullptr);
    }

    m_renderFinishedSemaphores.clear();
    m_imageAvailableSemaphores.clear();
    m_inFlightFences.clear();

    m_vertexBuffer.reset();
    m_indexBuffer.reset();
    m_uniformBuffers.clear();

    if (m_descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(m_vulkanDevice->getDevice(), m_descriptorPool, nullptr);
        m_descriptorPool = VK_NULL_HANDLE;
    }
    cleanupSwapChain();

    if (m_commandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(m_vulkanDevice->getDevice(), m_commandPool, nullptr);
        m_commandPool = VK_NULL_HANDLE;
    }

}
}
