target_sources(QiEngine PRIVATE
    vulkan/VulkanRenderer.cpp
    vulkan/VulkanBuffer.cpp
    vulkan/VulkanVertexBuffer.cpp
    vulkan/VulkanCommands.cpp
    vulkan/VulkanIndexBuffer.cpp
    vulkan/VulkanUniformBuffer.cpp
    vulkan/VulkanImage.cpp
    vulkan/VulkanInstance.cpp
    vulkan/VulkanSurface.cpp
    vulkan/VulkanDevice.cpp
    vulkan/VulkanSwapChain.cpp
    vulkan/VulkanRenderPass.cpp
    vulkan/VulkanGraphicsPipeline.cpp
    vulkan/VulkanTexture.cpp
    vulkan/VulkanSampler.cpp
    vulkan/VulkanTexture.cpp
    vulkan/Texture2D.cpp
    vulkan/VulkanAccelerationStructure.cpp
    vulkan/VulkanRTPipeline.cpp
    utils/VulkanUtils.cpp
    utils/Primitives.cpp

    Renderer.cpp
    Renderer2D.cpp
    Renderer3D.cpp

    # opengl/OpenGLRenderer.cpp
    # opengl/OpenGLShader.cpp
    # opengl/OpenGLVertexBuffer.cpp
    # opengl/OpenGLIndexBuffer.cpp
    # opengl/OpenGLVertexArray.cpp
    # opengl/OpenGLUniformBuffer.cpp

)
