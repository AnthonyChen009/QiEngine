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
    utils/VulkanUtils.cpp


    Renderer.cpp
    Renderer2D.cpp
    opengl/OpenGLRenderer.cpp
    opengl/OpenGLShader.cpp
    opengl/OpenGLVertexBuffer.cpp
    opengl/OpenGLIndexBuffer.cpp
    opengl/OpenGLVertexArray.cpp
    opengl/OpenGLUniformBuffer.cpp

)
