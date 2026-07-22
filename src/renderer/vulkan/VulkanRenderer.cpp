#include "SDL3/SDL_video.h"
#include "core/Base.hpp"
#include "core/Window.hpp"
#include "renderer/types/RTCameraUBO.hpp"
#include "renderer/types/SkyUbo.hpp"
#include "renderer/utils/VulkanUtils.hpp"
#include "renderer/vulkan/Texture2D.hpp"
#include "renderer/vulkan/VulkanGraphicsPipeline.hpp"
#include "renderer/vulkan/VulkanImage.hpp"
#include "renderer/vulkan/VulkanRenderer.hpp"
#include <cstdint>
#include <memory>
#include <vector>
#include <vulkan/vulkan.h>
#include "core/FileSystem.hpp"
#include "core/Assert.hpp"
#include "renderer/vulkan/VulkanSampler.hpp"
#include "types/Vertex.hpp"
#include <glm/glm.hpp>
#include "VulkanCommands.hpp"
#include "renderer/types/UniformBufferObject.hpp"
#include "types/PushConstants.hpp"
#include "imgui.h"
#include "imgui_impl_sdl3.h"
#include "imgui_impl_vulkan.h"

#define GLM_FORCE_DEPTH_ZERO_TO_ONE

namespace Qi {

static void checkVkResult(VkResult result) {
    if (result == VK_SUCCESS) return;
    QI_CORE_ERROR("[ImGui] Vulkan error code: {0}", static_cast<int>(result));
    if (result < 0)
        QI_RENDERER_ASSERT(false, "Aborted due to ImGui Vulkan error!");
}

void VulkanRenderer::init(Window& window) {
    m_window = &window;
    createInstance(window.getWindowName());
    m_surface.emplace(m_instance.getVkInstance(), window);
    m_vulkanDevice.emplace(m_instance.getVkInstance(), m_surface->getVkSurface());
    m_swapChain.emplace(m_vulkanDevice->getDevice(), m_vulkanDevice->getPhysicalDevice(), m_surface->getVkSurface(), window, m_vulkanDevice->findQueueFamilies(m_vulkanDevice->getPhysicalDevice()));
    m_renderPass.emplace(m_vulkanDevice->getDevice(), m_vulkanDevice->getPhysicalDevice(), m_swapChain->getImageFormat());
    m_graphicsPipeline2D.emplace(m_vulkanDevice->getDevice(), m_renderPass->getRenderPass(), VulkanUtils::PipelineType::Pipeline2D);
    m_graphicsPipeline3D.emplace(m_vulkanDevice->getDevice(), m_renderPass->getRenderPass(), VulkanUtils::PipelineType::Pipeline3D);
    m_graphicsPipelineSky.emplace(m_vulkanDevice->getDevice(), m_renderPass->getRenderPass(), VulkanUtils::PipelineType::PipelineSky);
    createCommandPool();
    createDepthResources();

    if (m_vulkanDevice->hasRTSupport()) {
        createRTOutputImage();
    }

    createFrameBuffers();
    createUniformBuffers();
    createDescriptorPool();
    createImGuiDescriptorPool();
    createCommandBuffers();
    createSyncObjects();
    createDescriptorSets(VulkanUtils::PipelineType::Pipeline2D);
    createDescriptorSets(VulkanUtils::PipelineType::Pipeline3D);
    createDescriptorSets(VulkanUtils::PipelineType::PipelineSky);

    if (m_vulkanDevice->hasRTSupport()) {
        m_graphicsPipelineRTDisplay.emplace(m_vulkanDevice->getDevice(), m_renderPass->getRenderPass(), VulkanUtils::PipelineType::PipelineRTDisplay);

        std::vector<VkDescriptorSetLayout> rtDisplayLayouts(MAX_FRAMES_IN_FLIGHT, m_graphicsPipelineRTDisplay->getDescriptorSetLayout());
        VkDescriptorSetAllocateInfo rtDisplayAllocInfo{};
        rtDisplayAllocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
        rtDisplayAllocInfo.descriptorPool = m_descriptorPool;
        rtDisplayAllocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
        rtDisplayAllocInfo.pSetLayouts = rtDisplayLayouts.data();
        m_descriptorSetsRTDisplay.resize(MAX_FRAMES_IN_FLIGHT);
        VkResult rtDisplayResult = vkAllocateDescriptorSets(m_vulkanDevice->getDevice(), &rtDisplayAllocInfo, m_descriptorSetsRTDisplay.data());
        QI_RENDERER_ASSERT(rtDisplayResult == VK_SUCCESS, "Failed to allocate RT display descriptor sets!");

        m_rtPipeline.emplace(m_vulkanDevice->getDevice());
        m_rtPipeline->buildSBT(m_vulkanDevice->getPhysicalDevice(), m_commandPool, m_vulkanDevice->getGraphicsQueue());
        createRTDescriptorSets();
    }

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
    vkWaitForFences(m_vulkanDevice->getDevice(), 1, &m_inFlightFences[m_currentFrame], VK_TRUE, UINT64_MAX);

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

    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    VkResult result = vkBeginCommandBuffer(m_commandBuffers[m_currentFrame], &beginInfo);
    QI_RENDERER_ASSERT(result == VK_SUCCESS, "Failed to begin recording command buffer!");

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
        m_renderFinishedSemaphores[m_currentImageIndex]
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
    if (result != VK_SUCCESS) {
        QI_CORE_ERROR("vkQueueSubmit failed: {}", static_cast<int>(result));
    }
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

    if (m_vsyncTogglePending) {
        vkDeviceWaitIdle(m_vulkanDevice->getDevice());
        m_swapChain->setVSync(m_pendingVSync);
        recreateSwapChain();
        m_vsyncTogglePending = false;
    }
}

void VulkanRenderer::dispatchRayTracing() {
    if (!m_vulkanDevice->hasRTSupport() || !m_rtDescriptorSetsValid[m_currentFrame]) return;

    VkCommandBuffer commandBuffer = m_commandBuffers[m_currentFrame];

    if (m_rtOutputSampledLastFrame) {
        VkImageMemoryBarrier resetBarrier{};
        resetBarrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
        resetBarrier.oldLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
        resetBarrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
        resetBarrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        resetBarrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
        resetBarrier.image = m_rtOutputImage;
        resetBarrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
        resetBarrier.srcAccessMask = VK_ACCESS_SHADER_READ_BIT;
        resetBarrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;

        vkCmdPipelineBarrier(
            commandBuffer,
            VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
            VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR,
            0, 0, nullptr, 0, nullptr, 1, &resetBarrier
        );
    }

    vkCmdBindPipeline(commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR, m_rtPipeline->getPipeline());
    vkCmdBindDescriptorSets(
        commandBuffer, VK_PIPELINE_BIND_POINT_RAY_TRACING_KHR,
        m_rtPipeline->getPipelineLayout(), 0, 1,
        &m_rtDescriptorSets[m_currentFrame], 0, nullptr
    );

    VkExtent2D extent = m_swapChain->getExtent();
    m_rtPipeline->cmdTraceRays(commandBuffer, extent.width, extent.height);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_GENERAL;
    barrier.newLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_rtOutputImage;
    barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    barrier.srcAccessMask = VK_ACCESS_SHADER_WRITE_BIT;
    barrier.dstAccessMask = VK_ACCESS_SHADER_READ_BIT;

    vkCmdPipelineBarrier(
        commandBuffer,
        VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR,
        VK_PIPELINE_STAGE_FRAGMENT_SHADER_BIT,
        0, 0, nullptr, 0, nullptr, 1, &barrier
    );

    updateRTOutputBindingFor3D();
    updateRTDisplayBinding();
    m_rtOutputSampledLastFrame = true;
}

void VulkanRenderer::onWindowResize(uint32_t width, uint32_t height) {
    m_winHeight = height;
    m_winWidth = width;
    m_frameBufferResized = true;

}

void VulkanRenderer::onVysncToggle(bool isVSync) {
    m_pendingVSync = isVSync;
    m_vsyncTogglePending = true;
}

void VulkanRenderer::drawIndexed(uint32_t count)  {
    QI_RENDERER_ASSERT(m_pipelineBound, "Cannot draw before binding a graphics pipeline!");
    QI_RENDERER_ASSERT(m_boundIndexBuffer, "Cannot draw: no index buffer bound!");

    vkCmdDrawIndexed(m_commandBuffers[m_currentFrame], m_boundIndexBuffer->getCount(), 1, 0, 0, 0);
}

void VulkanRenderer::createInstance(const std::string& appName) {
    m_instance.createVkInstance(appName);
    m_instance.setupDebugMessenger();
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

void VulkanRenderer::beginRenderPass() {
    VkCommandBuffer commandBuffer = m_commandBuffers[m_currentFrame];

    VkRenderPassBeginInfo renderPassInfo{};
    renderPassInfo.sType = VK_STRUCTURE_TYPE_RENDER_PASS_BEGIN_INFO;
    renderPassInfo.renderPass = m_renderPass->getRenderPass();
    renderPassInfo.framebuffer = m_swapChainFrameBuffers[m_currentImageIndex];
    renderPassInfo.renderArea.offset = {0, 0};
    renderPassInfo.renderArea.extent = m_swapChain->getExtent();
    std::array<VkClearValue, 2> clearValues{};
    clearValues[0].color = {{0.4980f, 0.6745f, 1.0f, 1.0f}};
    clearValues[1].depthStencil = {1.0f, 0};
    renderPassInfo.clearValueCount = static_cast<uint32_t>(clearValues.size());
    renderPassInfo.pClearValues = clearValues.data();
    vkCmdBeginRenderPass(commandBuffer, &renderPassInfo, VK_SUBPASS_CONTENTS_INLINE);

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

void VulkanRenderer::beginCommandBuffer(VkCommandBuffer commandBuffer, uint32_t imageIndex) {
    VkCommandBufferBeginInfo beginInfo{};
    beginInfo.sType = VK_STRUCTURE_TYPE_COMMAND_BUFFER_BEGIN_INFO;
    beginInfo.flags = 0;
    beginInfo.pInheritanceInfo = nullptr;
    VkResult result = vkBeginCommandBuffer(commandBuffer, &beginInfo);
    QI_RENDERER_ASSERT(result == VK_SUCCESS, "Failed to begin recording command buffer!");
}

void VulkanRenderer::createSyncObjects() {
    m_imageAvailableSemaphores.resize(MAX_FRAMES_IN_FLIGHT);
    m_renderFinishedSemaphores.resize(m_swapChain->getImages().size());
    m_inFlightFences.resize(MAX_FRAMES_IN_FLIGHT);

    VkSemaphoreCreateInfo semaphoreInfo{};
    semaphoreInfo.sType = VK_STRUCTURE_TYPE_SEMAPHORE_CREATE_INFO;

    VkFenceCreateInfo fenceInfo{};
    fenceInfo.sType = VK_STRUCTURE_TYPE_FENCE_CREATE_INFO;
    fenceInfo.flags = VK_FENCE_CREATE_SIGNALED_BIT;

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkResult imageSemaphoreResult = vkCreateSemaphore(m_vulkanDevice->getDevice(), &semaphoreInfo, nullptr, &m_imageAvailableSemaphores[i]);
        VkResult fenceResult = vkCreateFence(m_vulkanDevice->getDevice(), &fenceInfo, nullptr, &m_inFlightFences[i]);
        QI_RENDERER_ASSERT(imageSemaphoreResult == VK_SUCCESS && fenceResult == VK_SUCCESS, "Failed to create synchronization objects!");
    }

    for (size_t i = 0; i < m_renderFinishedSemaphores.size(); i++) {
        VkResult renderSemaphoreResult = vkCreateSemaphore(m_vulkanDevice->getDevice(), &semaphoreInfo, nullptr, &m_renderFinishedSemaphores[i]);
        QI_RENDERER_ASSERT(renderSemaphoreResult == VK_SUCCESS, "Failed to create render-finished semaphore!");
    }
}

void VulkanRenderer::bindPipeline(VulkanUtils::PipelineType type) {
    std::optional<VulkanGraphicsPipeline>* selPipeline = nullptr;
    std::vector<VkDescriptorSet>* sets = nullptr;

    switch (type) {
        case VulkanUtils::PipelineType::Pipeline2D:
            selPipeline = &m_graphicsPipeline2D;
            sets = &m_descriptorSets2D;
            break;
        case VulkanUtils::PipelineType::Pipeline3D:
            selPipeline = &m_graphicsPipeline3D;
            sets = &m_descriptorSets3D;
            break;
        case VulkanUtils::PipelineType::PipelineSky:
            selPipeline = &m_graphicsPipelineSky;
            sets = &m_descriptorSetsSky;
            break;
        case VulkanUtils::PipelineType::PipelineRTDisplay:
            selPipeline = &m_graphicsPipelineRTDisplay;
            sets = &m_descriptorSetsRTDisplay;
            break;
    }

    QI_RENDERER_ASSERT(selPipeline->has_value(), "Pipeline type has not been created!");
    vkCmdBindPipeline(m_commandBuffers[m_currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, (*selPipeline)->getPipeline());
    vkCmdBindDescriptorSets(m_commandBuffers[m_currentFrame], VK_PIPELINE_BIND_POINT_GRAPHICS, (*selPipeline)->getPipelineLayout(), 0, 1, &(*sets)[m_currentFrame], 0, nullptr);
    m_boundVertexBuffer = nullptr;
    m_boundIndexBuffer = nullptr;
    m_pipelineBound = true;
}

void VulkanRenderer::bindBuffers(const VertexBuffer& vertexBuffer, const IndexBuffer& indexBuffer) {
    QI_RENDERER_ASSERT(m_pipelineBound, "Cannot bind buffers before a graphics pipeline is bound!");
    QI_RENDERER_ASSERT(vertexBuffer.isBufferValid(), "Vertex buffer handle is invalid!");
    QI_RENDERER_ASSERT(indexBuffer.isBufferValid(), "Index buffer handle is invalid!");
    VkCommandBuffer commandBuffer = m_commandBuffers[m_currentFrame];

    vertexBuffer.bind(static_cast<CommandBufferHandle>(commandBuffer));
    indexBuffer.bind(static_cast<CommandBufferHandle>(commandBuffer));

    m_boundVertexBuffer = &vertexBuffer;
    m_boundIndexBuffer = &indexBuffer;
}

void VulkanRenderer::updateRTDisplayBinding() {
    if (!m_vulkanDevice->hasRTSupport()) return;
    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL;
    imageInfo.imageView = m_rtOutputImageView;
    imageInfo.sampler = m_rtOutputSampler;
    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = m_descriptorSetsRTDisplay[m_currentFrame];
    write.dstBinding = 0;
    write.dstArrayElement = 0;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.descriptorCount = 1;
    write.pImageInfo = &imageInfo;
    vkUpdateDescriptorSets(m_vulkanDevice->getDevice(), 1, &write, 0, nullptr);
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

    if (m_vulkanDevice->hasRTSupport()) {
        cleanupRTOutputImage();
        m_rtOutputSampledLastFrame = false;
        std::fill(m_rtDescriptorSetsValid.begin(), m_rtDescriptorSetsValid.end(), false);
    }
}

void VulkanRenderer::recreateSwapChain() {

    m_window->waitForValidFramebufferSize();

    vkDeviceWaitIdle(m_vulkanDevice->getDevice());

    cleanupSwapChain();

    m_swapChain->recreate(*m_window);
    createDepthResources();
    createFrameBuffers();
    if (m_vulkanDevice->hasRTSupport()) {
        createRTOutputImage();
    }
}

void VulkanRenderer::createUniformBuffers() {
    m_uniformBuffers2D.resize(MAX_FRAMES_IN_FLIGHT);
    m_uniformBuffers3D.resize(MAX_FRAMES_IN_FLIGHT);
    m_skyUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    m_rtCameraUniformBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    m_rtDescriptorSetsValid.resize(MAX_FRAMES_IN_FLIGHT, false);
    m_instanceAddressesBuffers.resize(MAX_FRAMES_IN_FLIGHT);
    m_tlas.resize(MAX_FRAMES_IN_FLIGHT);

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        m_uniformBuffers2D[i] = std::make_unique<VulkanUniformBuffer>(m_vulkanDevice->getDevice(), m_vulkanDevice->getPhysicalDevice(), sizeof(UniformBufferObject));
        m_uniformBuffers3D[i] = std::make_unique<VulkanUniformBuffer>(m_vulkanDevice->getDevice(), m_vulkanDevice->getPhysicalDevice(), sizeof(UniformBufferObject));
        m_skyUniformBuffers[i] = std::make_unique<VulkanUniformBuffer>(m_vulkanDevice->getDevice(), m_vulkanDevice->getPhysicalDevice(), sizeof(SkyUniformBufferObject));
        m_rtCameraUniformBuffers[i] = std::make_unique<VulkanUniformBuffer>(m_vulkanDevice->getDevice(), m_vulkanDevice->getPhysicalDevice(), sizeof(RTCameraUBO));
    }
}

void VulkanRenderer::updateUniformBuffer2D() {
    UniformBufferObject ubo{};
    ubo.view = glm::mat4(1.0f);
    float width = static_cast<float>(m_swapChain->getExtent().width);
    float height = static_cast<float>(m_swapChain->getExtent().height);
    ubo.proj = glm::ortho(-width/2.0f, width/2.0f, -height/2.0f, height/2.0f, -1.0f, 1.0f);
    ubo.proj[1][1] *= -1;
    m_uniformBuffers2D[m_currentFrame]->setData(&ubo, sizeof(ubo));
}

void VulkanRenderer::updateUniformBuffer3D(UniformBufferObject& ubo) {
    m_uniformBuffers3D[m_currentFrame]->setData(&ubo, sizeof(ubo));
}

void VulkanRenderer::updateUniformBufferRT(RTCameraUBO& ubo) {
    m_rtCameraUniformBuffers[m_currentFrame]->setData(&ubo, sizeof(ubo));
}

void VulkanRenderer::updateUniformBufferSky(SkyUniformBufferObject& ubo) {
    m_skyUniformBuffers[m_currentFrame]->setData(&ubo, sizeof(ubo));
}

void VulkanRenderer::createDescriptorPool() {
    std::array<VkDescriptorPoolSize, 5> poolSizes{};
    poolSizes[0].type = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    poolSizes[0].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 8); // generous headroom
    poolSizes[1].type = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    poolSizes[1].descriptorCount = 1024 * static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 4); // +1 for RT-output binding on Pipeline3D
    poolSizes[2].type = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    poolSizes[2].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 2);
    poolSizes[3].type = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    poolSizes[3].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 2);
    poolSizes[4].type = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    poolSizes[4].descriptorCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 2);

    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_UPDATE_AFTER_BIND_BIT;
    poolInfo.poolSizeCount = static_cast<uint32_t>(poolSizes.size());
    poolInfo.pPoolSizes = poolSizes.data();
    poolInfo.maxSets = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT * 8); // generous headroom for set count too

    VkResult result = vkCreateDescriptorPool(m_vulkanDevice->getDevice(), &poolInfo, nullptr, &m_descriptorPool);
    QI_RENDERER_ASSERT(result == VK_SUCCESS, "Failed to create descriptor pool!");
}

void VulkanRenderer::createDescriptorSets(VulkanUtils::PipelineType type) {
    std::optional<VulkanGraphicsPipeline>* selPipeline = nullptr;
    std::vector<VkDescriptorSet>* sets = nullptr;
    std::vector<std::unique_ptr<VulkanUniformBuffer>>* uniformBuffers = nullptr;
    VkDeviceSize uboSize = 0;

    switch (type) {
        case VulkanUtils::PipelineType::Pipeline2D:
            selPipeline = &m_graphicsPipeline2D;
            sets = &m_descriptorSets2D;
            uniformBuffers = &m_uniformBuffers2D;
            uboSize = sizeof(UniformBufferObject);
            break;
        case VulkanUtils::PipelineType::Pipeline3D:
            selPipeline = &m_graphicsPipeline3D;
            sets = &m_descriptorSets3D;
            uniformBuffers = &m_uniformBuffers3D;
            uboSize = sizeof(UniformBufferObject);
            break;
        case VulkanUtils::PipelineType::PipelineSky:
            selPipeline = &m_graphicsPipelineSky;
            sets = &m_descriptorSetsSky;
            uniformBuffers = &m_skyUniformBuffers;
            uboSize = sizeof(SkyUniformBufferObject);
            break;
    }

    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, (*selPipeline)->getDescriptorSetLayout());
    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();
    sets->resize(MAX_FRAMES_IN_FLIGHT);
    VkResult result = vkAllocateDescriptorSets(m_vulkanDevice->getDevice(), &allocInfo, sets->data());
    QI_RENDERER_ASSERT(result == VK_SUCCESS, "Failed to allocate descriptor sets!");

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        VkDescriptorBufferInfo bufferInfo{};
        bufferInfo.buffer = (*uniformBuffers)[i]->getBuffer();
        bufferInfo.offset = 0;
        bufferInfo.range = uboSize;
        VkWriteDescriptorSet write{};
        write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
        write.dstSet = (*sets)[i];
        write.dstBinding = 0;
        write.dstArrayElement = 0;
        write.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
        write.descriptorCount = 1;
        write.pBufferInfo = &bufferInfo;
        vkUpdateDescriptorSets(m_vulkanDevice->getDevice(), 1, &write, 0, nullptr);
    }
}

void VulkanRenderer::createRTDescriptorSets() {
    std::vector<VkDescriptorSetLayout> layouts(MAX_FRAMES_IN_FLIGHT, m_rtPipeline->getDescriptorSetLayout());

    VkDescriptorSetAllocateInfo allocInfo{};
    allocInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_SET_ALLOCATE_INFO;
    allocInfo.descriptorPool = m_descriptorPool;
    allocInfo.descriptorSetCount = static_cast<uint32_t>(MAX_FRAMES_IN_FLIGHT);
    allocInfo.pSetLayouts = layouts.data();

    m_rtDescriptorSets.resize(MAX_FRAMES_IN_FLIGHT);
    VkResult result = vkAllocateDescriptorSets(m_vulkanDevice->getDevice(), &allocInfo, m_rtDescriptorSets.data());
    QI_CORE_ERROR("vkAllocateDescriptorSets (RT) returned: {}", static_cast<int>(result));
    QI_RENDERER_ASSERT(result == VK_SUCCESS, "Failed to allocate RT descriptor sets!");
}

void VulkanRenderer::updateRTDescriptorSet() {
    QI_RENDERER_ASSERT(m_tlas[m_currentFrame] != nullptr, "updateRTDescriptorSet called before TLAS was built for this frame");
    QI_RENDERER_ASSERT(m_instanceAddressesBuffers[m_currentFrame] != nullptr, "updateRTDescriptorSet called before instance addresses buffer was built for this frame");

    VkAccelerationStructureKHR tlasHandle = m_tlas[m_currentFrame]->getHandle();

    VkWriteDescriptorSetAccelerationStructureKHR asWrite{};
    asWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET_ACCELERATION_STRUCTURE_KHR;
    asWrite.accelerationStructureCount = 1;
    asWrite.pAccelerationStructures = &tlasHandle;

    VkWriteDescriptorSet tlasWrite{};
    tlasWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    tlasWrite.pNext = &asWrite;
    tlasWrite.dstSet = m_rtDescriptorSets[m_currentFrame];
    tlasWrite.dstBinding = 0;
    tlasWrite.dstArrayElement = 0;
    tlasWrite.descriptorType = VK_DESCRIPTOR_TYPE_ACCELERATION_STRUCTURE_KHR;
    tlasWrite.descriptorCount = 1;

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageView = m_rtOutputImageView;
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_GENERAL;

    VkWriteDescriptorSet imageWrite{};
    imageWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    imageWrite.dstSet = m_rtDescriptorSets[m_currentFrame];
    imageWrite.dstBinding = 1;
    imageWrite.dstArrayElement = 0;
    imageWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_IMAGE;
    imageWrite.descriptorCount = 1;
    imageWrite.pImageInfo = &imageInfo;

    VkDescriptorBufferInfo bufferInfo{};
    bufferInfo.buffer = m_rtCameraUniformBuffers[m_currentFrame]->getBuffer();
    bufferInfo.offset = 0;
    bufferInfo.range = sizeof(RTCameraUBO);

    VkWriteDescriptorSet uboWrite{};
    uboWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    uboWrite.dstSet = m_rtDescriptorSets[m_currentFrame];
    uboWrite.dstBinding = 2;
    uboWrite.dstArrayElement = 0;
    uboWrite.descriptorType = VK_DESCRIPTOR_TYPE_UNIFORM_BUFFER;
    uboWrite.descriptorCount = 1;
    uboWrite.pBufferInfo = &bufferInfo;

    VkDescriptorBufferInfo instanceAddressesBufferInfo{};
    instanceAddressesBufferInfo.buffer = m_instanceAddressesBuffers[m_currentFrame]->getBuffer();
    instanceAddressesBufferInfo.offset = 0;
    instanceAddressesBufferInfo.range = VK_WHOLE_SIZE;

    VkWriteDescriptorSet instanceAddressesWrite{};
    instanceAddressesWrite.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    instanceAddressesWrite.dstSet = m_rtDescriptorSets[m_currentFrame];
    instanceAddressesWrite.dstBinding = 3;
    instanceAddressesWrite.dstArrayElement = 0;
    instanceAddressesWrite.descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER;
    instanceAddressesWrite.descriptorCount = 1;
    instanceAddressesWrite.pBufferInfo = &instanceAddressesBufferInfo;

    std::array<VkWriteDescriptorSet, 4> writes = { tlasWrite, imageWrite, uboWrite, instanceAddressesWrite };
    vkUpdateDescriptorSets(m_vulkanDevice->getDevice(), static_cast<uint32_t>(writes.size()), writes.data(), 0, nullptr);
    m_rtDescriptorSetsValid[m_currentFrame] = true;
}

void VulkanRenderer::createDepthResources() {
    VkFormat depthFormat = VulkanUtils::findDepthFormat(m_vulkanDevice->getPhysicalDevice());
    VulkanImage::createImage(m_vulkanDevice->getDevice(), m_vulkanDevice->getPhysicalDevice(), m_swapChain->getExtent().width, m_swapChain->getExtent().height, depthFormat, VK_IMAGE_TILING_OPTIMAL, VK_IMAGE_USAGE_DEPTH_STENCIL_ATTACHMENT_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT, m_depthImage, m_depthImageMemory);
    m_depthImageView = VulkanImage::createImageView(m_vulkanDevice->getDevice(), m_depthImage, depthFormat, VK_IMAGE_ASPECT_DEPTH_BIT);
    VulkanCommands::transitionImageLayout(m_vulkanDevice->getDevice(), m_commandPool, m_vulkanDevice->getPresentQueue(), m_depthImage, depthFormat, VK_IMAGE_LAYOUT_UNDEFINED, VK_IMAGE_LAYOUT_DEPTH_STENCIL_ATTACHMENT_OPTIMAL);
}

void VulkanRenderer::createRTOutputImage() {
    VkExtent2D extent = m_swapChain->getExtent();
    m_rtOutputSampler = VulkanSampler::create(m_vulkanDevice->getDevice(), m_vulkanDevice->getPhysicalDevice());

    VulkanImage::createImage(
        m_vulkanDevice->getDevice(),
        m_vulkanDevice->getPhysicalDevice(),
        extent.width, extent.height,
        VK_FORMAT_R8G8B8A8_UNORM,
        VK_IMAGE_TILING_OPTIMAL,
        VK_IMAGE_USAGE_STORAGE_BIT | VK_IMAGE_USAGE_SAMPLED_BIT,
        VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT,
        m_rtOutputImage,
        m_rtOutputImageMemory
    );

    m_rtOutputImageView = VulkanImage::createImageView(m_vulkanDevice->getDevice(), m_rtOutputImage, VK_FORMAT_R8G8B8A8_UNORM, VK_IMAGE_ASPECT_COLOR_BIT);
    VkCommandBuffer cmd = VulkanCommands::beginSingleTimeCommands(m_vulkanDevice->getDevice(), m_commandPool);

    VkImageMemoryBarrier barrier{};
    barrier.sType = VK_STRUCTURE_TYPE_IMAGE_MEMORY_BARRIER;
    barrier.oldLayout = VK_IMAGE_LAYOUT_UNDEFINED;
    barrier.newLayout = VK_IMAGE_LAYOUT_GENERAL;
    barrier.srcQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.dstQueueFamilyIndex = VK_QUEUE_FAMILY_IGNORED;
    barrier.image = m_rtOutputImage;
    barrier.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };
    barrier.srcAccessMask = 0;
    barrier.dstAccessMask = VK_ACCESS_SHADER_WRITE_BIT;

    vkCmdPipelineBarrier(
        cmd,
        VK_PIPELINE_STAGE_TOP_OF_PIPE_BIT,
        VK_PIPELINE_STAGE_RAY_TRACING_SHADER_BIT_KHR,
        0, 0, nullptr, 0, nullptr, 1, &barrier
    );

    VulkanCommands::endSingleTimeCommands(m_vulkanDevice->getDevice(), m_commandPool, m_vulkanDevice->getGraphicsQueue(), cmd);
}

void VulkanRenderer::cleanupRTOutputImage() {
    if (m_rtOutputImageView != VK_NULL_HANDLE) {
        vkDestroyImageView(m_vulkanDevice->getDevice(), m_rtOutputImageView, nullptr);
        m_rtOutputImageView = VK_NULL_HANDLE;
    }
    if (m_rtOutputImage != VK_NULL_HANDLE) {
        vkDestroyImage(m_vulkanDevice->getDevice(), m_rtOutputImage, nullptr);
        m_rtOutputImage = VK_NULL_HANDLE;
    }
    if (m_rtOutputImageMemory != VK_NULL_HANDLE) {
        vkFreeMemory(m_vulkanDevice->getDevice(), m_rtOutputImageMemory, nullptr);
        m_rtOutputImageMemory = VK_NULL_HANDLE;
    }
    if (m_rtOutputSampler != VK_NULL_HANDLE) {
        VulkanSampler::destroy(m_vulkanDevice->getDevice(), m_rtOutputSampler);
    }
}

bool VulkanRenderer::hasStencilComponent(VkFormat format) {
    return format == VK_FORMAT_D32_SFLOAT_S8_UINT || format == VK_FORMAT_D24_UNORM_S8_UINT;
}

void VulkanRenderer::pushConstants2D(const PushConstant& push) {
    vkCmdPushConstants(
        m_commandBuffers[m_currentFrame],
        m_graphicsPipeline2D->getPipelineLayout(),
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        0,
        sizeof(PushConstant),
        &push
    );
}

void VulkanRenderer::pushConstants3D(const PushConstant& push) {
    vkCmdPushConstants(
        m_commandBuffers[m_currentFrame],
        m_graphicsPipeline3D->getPipelineLayout(),
        VK_SHADER_STAGE_VERTEX_BIT | VK_SHADER_STAGE_FRAGMENT_BIT,
        0,
        sizeof(PushConstant),
        &push
    );
}

std::shared_ptr<Texture2D> VulkanRenderer::getOrLoadTexture(const std::string& path) {

    auto it = m_textureCache.find(path);
    if (it != m_textureCache.end())
        return it->second;

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
        write.dstBinding = 1;
        write.dstArrayElement = index;
        write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
        write.descriptorCount = 1;
        write.pImageInfo = &imageInfo;

        write.dstSet = m_descriptorSets2D[i];
        vkUpdateDescriptorSets(m_vulkanDevice->getDevice(), 1, &write, 0, nullptr);
        write.dstSet = m_descriptorSets3D[i];
        vkUpdateDescriptorSets(m_vulkanDevice->getDevice(), 1, &write, 0, nullptr);
    }

    m_textureCache[path] = std::move(texture);
    return m_textureCache[path];
}

std::shared_ptr<VertexBuffer> VulkanRenderer::createVertexBuffer(const std::vector<Vertex>& vertices) {
    return std::make_shared<VulkanVertexBuffer>(m_vulkanDevice->getDevice(), m_vulkanDevice->getPhysicalDevice(), m_commandPool, m_vulkanDevice->getGraphicsQueue(), vertices, m_vulkanDevice->hasRTSupport());
}

std::shared_ptr<IndexBuffer> VulkanRenderer::createIndexBuffer(const std::vector<uint32_t>& indices) {
    return std::make_shared<VulkanIndexBuffer>(m_vulkanDevice->getDevice(), m_vulkanDevice->getPhysicalDevice(), m_commandPool, m_vulkanDevice->getGraphicsQueue(), indices, m_vulkanDevice->hasRTSupport());
}

std::unique_ptr<VulkanAccelerationStructure> VulkanRenderer::createBLAS(const VertexBuffer& vertexBuffer, uint32_t vertexCount, size_t vertexStride, const IndexBuffer& indexBuffer, uint32_t indexCount, bool allowUpdate) {
    QI_RENDERER_ASSERT(m_vulkanDevice.has_value(), "VulkanDevice not initialized");
    QI_RENDERER_ASSERT(vertexCount > 0 && indexCount > 0, "Cannot build acceleration structure with empty geometry");
    const VulkanVertexBuffer* vkVertexBuffer = dynamic_cast<const VulkanVertexBuffer*>(&vertexBuffer);
    const VulkanIndexBuffer* vkIndexBuffer = dynamic_cast<const VulkanIndexBuffer*>(&indexBuffer);
    QI_RENDERER_ASSERT(vkVertexBuffer && vkIndexBuffer, "Expected Vulkan buffer types for acceleration structure creation");

    std::unique_ptr<VulkanAccelerationStructure> blas = std::make_unique<VulkanAccelerationStructure>(m_vulkanDevice->getDevice(), m_vulkanDevice->getPhysicalDevice());
    blas->buildBLAS(
        m_commandPool,
        m_vulkanDevice->getGraphicsQueue(),
        vkVertexBuffer->getVulkanBuffer(), vertexCount, vertexStride,
        vkIndexBuffer->getVulkanBuffer(), indexCount,
        allowUpdate
    );
    return blas;
}

void VulkanRenderer::updateTLAS(const std::vector<RTInstanceData>& instances) {
    if (instances.empty()) return;

    std::vector<VkAccelerationStructureInstanceKHR> vkInstances(instances.size());
    std::vector<InstanceAddresses> instanceAddresses(instances.size());

    for (size_t i = 0; i < instances.size(); ++i) {
        const auto& inst = instances[i];
        VkTransformMatrixKHR transform{};
        // glm is column-major; VkTransformMatrixKHR wants row-major, top 3 rows
        for (int row = 0; row < 3; ++row)
            for (int col = 0; col < 4; ++col)
                transform.matrix[row][col] = inst.transform[col][row];

        vkInstances[i].transform = transform;
        vkInstances[i].instanceCustomIndex = inst.instanceCustomIndex;
        vkInstances[i].mask = inst.mask;
        vkInstances[i].instanceShaderBindingTableRecordOffset = 0;
        vkInstances[i].flags = VK_GEOMETRY_INSTANCE_TRIANGLE_FACING_CULL_DISABLE_BIT_KHR;
        vkInstances[i].accelerationStructureReference = inst.blasAddress;

        instanceAddresses[i].vertexBufferAddress = inst.vertexBufferAddress;
        instanceAddresses[i].indexBufferAddress = inst.indexBufferAddress;
    }

    VkDeviceSize bufferSize = sizeof(VkAccelerationStructureInstanceKHR) * vkInstances.size();

    VulkanBuffer stagingBuffer(m_vulkanDevice->getDevice(), m_vulkanDevice->getPhysicalDevice());
    stagingBuffer.create(bufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    stagingBuffer.setData(vkInstances.data(), bufferSize);

    auto instanceBuffer = std::make_unique<VulkanBuffer>(m_vulkanDevice->getDevice(), m_vulkanDevice->getPhysicalDevice());
    instanceBuffer->create(bufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_ACCELERATION_STRUCTURE_BUILD_INPUT_READ_ONLY_BIT_KHR | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);

    VulkanCommands::copyBuffer(m_vulkanDevice->getDevice(), m_commandPool, m_vulkanDevice->getGraphicsQueue(), stagingBuffer.getBuffer(), instanceBuffer->getBuffer(), bufferSize);

    m_tlas[m_currentFrame] = std::make_unique<VulkanAccelerationStructure>(m_vulkanDevice->getDevice(), m_vulkanDevice->getPhysicalDevice());

    m_tlas[m_currentFrame]->buildTLAS(m_commandPool, m_vulkanDevice->getGraphicsQueue(), *instanceBuffer, static_cast<uint32_t>(vkInstances.size()), false);

    VkDeviceSize addrBufferSize = sizeof(InstanceAddresses) * instanceAddresses.size();
    VulkanBuffer addrStagingBuffer(m_vulkanDevice->getDevice(), m_vulkanDevice->getPhysicalDevice());
    addrStagingBuffer.create(addrBufferSize, VK_BUFFER_USAGE_TRANSFER_SRC_BIT, VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT | VK_MEMORY_PROPERTY_HOST_COHERENT_BIT);
    addrStagingBuffer.setData(instanceAddresses.data(), addrBufferSize);

    m_instanceAddressesBuffers[m_currentFrame] = std::make_unique<VulkanBuffer>(m_vulkanDevice->getDevice(), m_vulkanDevice->getPhysicalDevice());
    m_instanceAddressesBuffers[m_currentFrame]->create(addrBufferSize, VK_BUFFER_USAGE_TRANSFER_DST_BIT | VK_BUFFER_USAGE_STORAGE_BUFFER_BIT | VK_BUFFER_USAGE_SHADER_DEVICE_ADDRESS_BIT, VK_MEMORY_PROPERTY_DEVICE_LOCAL_BIT);
    VulkanCommands::copyBuffer(m_vulkanDevice->getDevice(), m_commandPool, m_vulkanDevice->getGraphicsQueue(), addrStagingBuffer.getBuffer(), m_instanceAddressesBuffers[m_currentFrame]->getBuffer(), addrBufferSize);
    // instanceBuffer can be released after the build completes — the AS build reads it
    // synchronously within this function via the single-time command submission, so it's
    // safe to let this unique_ptr go out of scope here.
}

void VulkanRenderer::updateRTOutputBindingFor3D() {
    if (!m_vulkanDevice->hasRTSupport()) return;

    VkDescriptorImageInfo imageInfo{};
    imageInfo.imageLayout = VK_IMAGE_LAYOUT_SHADER_READ_ONLY_OPTIMAL; // matches the post-trace transition
    imageInfo.imageView = m_rtOutputImageView;
    imageInfo.sampler = m_rtOutputSampler; // see note below

    VkWriteDescriptorSet write{};
    write.sType = VK_STRUCTURE_TYPE_WRITE_DESCRIPTOR_SET;
    write.dstSet = m_descriptorSets3D[m_currentFrame];
    write.dstBinding = 2;
    write.dstArrayElement = 0;
    write.descriptorType = VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER;
    write.descriptorCount = 1;
    write.pImageInfo = &imageInfo;

    vkUpdateDescriptorSets(m_vulkanDevice->getDevice(), 1, &write, 0, nullptr);
}

void VulkanRenderer::initImGui(Window* window) {
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();

    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;
    //io.ConfigFlags |= ImGuiConfigFlags_NoMouseCursorChange;


    ImGui::StyleColorsDark();
    SDL_Window* nativeWindow = static_cast<SDL_Window*>(window->getNativeWindow());

    ImGui_ImplVulkan_InitInfo initInfo{};
    initInfo.ApiVersion = VK_API_VERSION_1_4;
    initInfo.Instance = m_instance.getVkInstance();
    initInfo.PhysicalDevice = m_vulkanDevice->getPhysicalDevice();
    initInfo.Device = m_vulkanDevice->getDevice();
    //fix this
    initInfo.QueueFamily = m_vulkanDevice->findQueueFamilies(m_vulkanDevice->getPhysicalDevice()).graphicsFamily.value();
    initInfo.Queue = m_vulkanDevice->getGraphicsQueue();
    initInfo.DescriptorPool = m_imguiDescriptorPool;
    initInfo.MinImageCount = static_cast<uint32_t>(m_swapChain->getImages().size());
    initInfo.ImageCount = static_cast<uint32_t>(m_swapChain->getImages().size());

    initInfo.PipelineInfoMain.RenderPass = m_renderPass->getRenderPass();
    initInfo.PipelineInfoMain.Subpass = 0;
    initInfo.PipelineInfoMain.MSAASamples = VK_SAMPLE_COUNT_1_BIT;

    initInfo.CheckVkResultFn = checkVkResult;

    ImGui_ImplSDL3_InitForVulkan(nativeWindow);
    ImGui_ImplVulkan_Init(&initInfo);
    window->setRawEventCallback([](void* e) {
        ImGui_ImplSDL3_ProcessEvent(static_cast<SDL_Event*>(e));
    });
}

void VulkanRenderer::shutdownImGui() {
    vkDeviceWaitIdle(m_vulkanDevice->getDevice());
    ImGui_ImplVulkan_Shutdown();
    ImGui_ImplSDL3_Shutdown();
    ImGui::DestroyContext();
}

void VulkanRenderer::beginImGuiFrame() {
    ImGui_ImplVulkan_NewFrame();
    ImGui_ImplSDL3_NewFrame();
    ImGui::NewFrame();
}

void VulkanRenderer::renderImGui() {
    ImGui::Render();
    ImGui_ImplVulkan_RenderDrawData(ImGui::GetDrawData(), m_commandBuffers[m_currentFrame]);
}

void VulkanRenderer::createImGuiDescriptorPool() {
    VkDescriptorPoolSize poolSizes[] = {
        { VK_DESCRIPTOR_TYPE_COMBINED_IMAGE_SAMPLER, 1000 }
    };
    VkDescriptorPoolCreateInfo poolInfo{};
    poolInfo.sType = VK_STRUCTURE_TYPE_DESCRIPTOR_POOL_CREATE_INFO;
    poolInfo.flags = VK_DESCRIPTOR_POOL_CREATE_FREE_DESCRIPTOR_SET_BIT;
    poolInfo.maxSets = 1000;
    poolInfo.poolSizeCount = 1;
    poolInfo.pPoolSizes = poolSizes;

    VkResult result = vkCreateDescriptorPool(m_vulkanDevice->getDevice(), &poolInfo, nullptr, &m_imguiDescriptorPool);
    QI_RENDERER_ASSERT(result == VK_SUCCESS, "Failed to create ImGui descriptor pool!");
}

void VulkanRenderer::drawFullscreenTriangle() {
    vkCmdDraw(m_commandBuffers[m_currentFrame], 3, 1, 0, 0);
}

void VulkanRenderer::waitIdle() {
    if (m_vulkanDevice->getDevice() != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(m_vulkanDevice->getDevice());
    }
}

void VulkanRenderer::shutdown() {
    if (m_vulkanDevice->getDevice() != VK_NULL_HANDLE) {
        vkDeviceWaitIdle(m_vulkanDevice->getDevice());
    }

    for (size_t i = 0; i < MAX_FRAMES_IN_FLIGHT; i++) {
        if (m_imageAvailableSemaphores[i] != VK_NULL_HANDLE)
            vkDestroySemaphore(m_vulkanDevice->getDevice(), m_imageAvailableSemaphores[i], nullptr);

        if (m_inFlightFences[i] != VK_NULL_HANDLE)
            vkDestroyFence(m_vulkanDevice->getDevice(), m_inFlightFences[i], nullptr);
    }

    for (size_t i = 0; i < m_renderFinishedSemaphores.size(); i++) {
        if (m_renderFinishedSemaphores[i] != VK_NULL_HANDLE)
            vkDestroySemaphore(m_vulkanDevice->getDevice(), m_renderFinishedSemaphores[i], nullptr);
    }

    m_renderFinishedSemaphores.clear();
    m_imageAvailableSemaphores.clear();
    m_inFlightFences.clear();

    m_boundVertexBuffer = nullptr;
    m_boundIndexBuffer = nullptr;
    m_uniformBuffers2D.clear();
    m_uniformBuffers3D.clear();


    if (m_descriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(m_vulkanDevice->getDevice(), m_descriptorPool, nullptr);
        m_descriptorPool = VK_NULL_HANDLE;
    }

    if (m_imguiDescriptorPool != VK_NULL_HANDLE) {
        vkDestroyDescriptorPool(m_vulkanDevice->getDevice(), m_imguiDescriptorPool, nullptr);
        m_imguiDescriptorPool = VK_NULL_HANDLE;
    }

    cleanupSwapChain();

    if (m_commandPool != VK_NULL_HANDLE) {
        vkDestroyCommandPool(m_vulkanDevice->getDevice(), m_commandPool, nullptr);
        m_commandPool = VK_NULL_HANDLE;
    }
}
}
