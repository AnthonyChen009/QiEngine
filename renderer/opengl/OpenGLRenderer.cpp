#include "OpenGLRenderer.hpp"
#include "Log.hpp"
#include "Assert.hpp"
#include "opengl/OpenGLVertexBuffer.hpp"
#include "types/Vertex.hpp"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <cstddef>
#include <memory>
#include "FileSystem.hpp"
#include "types/UniformBufferObject.hpp"

namespace Qi {

void OpenGLRenderer::init(Window& window) {
    m_window = &window;

    GLFWwindow* nativeWindow =
        static_cast<GLFWwindow*>(window.getNativeWindow());

    QI_CORE_ASSERT(nativeWindow, "OpenGLRenderer needs a valid GLFW window!");

    glfwMakeContextCurrent(nativeWindow);

    int success = gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);
    QI_CORE_ASSERT(success, "Failed to initialize GLAD!");

    glViewport(0, 0, window.getWidth(), window.getHeight());

    glEnable(GL_DEPTH_TEST);

    m_vertexArray = std::make_unique<OpenGLVertexArray>();
    m_vertexArray->bind();

    m_vertexBuffer = std::make_unique<OpenGLVertexBuffer>(vertices.data(), vertices.size() * sizeof(Vertex));
    m_vertexBuffer->bind();

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(
        0,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<const void*>(offsetof(Vertex, pos))
    );

    glEnableVertexAttribArray(1);
    glVertexAttribPointer(
        1,
        3,
        GL_FLOAT,
        GL_FALSE,
        sizeof(Vertex),
        reinterpret_cast<const void*>(offsetof(Vertex, color))
    );

    m_indexBuffer = std::make_unique<OpenGLIndexBuffer>(indices.data(), static_cast<uint32_t>(indices.size()));

    m_indexBuffer->bind();
    m_vertexArray->unbind();

    std::string vertexSrc = FileSystem::readTextFile("shaders/shader.vert");
    std::string fragmentSrc = FileSystem::readTextFile("shaders/shader.frag");

    m_shader = std::make_unique<OpenGLShader>(vertexSrc, fragmentSrc);

    m_uniformBuffer = std::make_unique<OpenGLUniformBuffer>( sizeof(UniformBufferObject), 0);
    QI_CORE_INFO("OpenGL initialized");
    QI_CORE_INFO("OpenGL Vendor: {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
    QI_CORE_INFO("OpenGL Renderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
    QI_CORE_INFO("OpenGL Version: {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
}

void OpenGLRenderer::shutdown() {
    m_uniformBuffer.reset();
    m_indexBuffer.reset();
    m_vertexBuffer.reset();
    m_vertexArray.reset();
    m_shader.reset();

    m_window = nullptr;

    QI_CORE_INFO("OpenGLRenderer shutdown");
}

bool OpenGLRenderer::beginFrame() {
    glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    return true;
}

void OpenGLRenderer::endFrame() {
    GLFWwindow* nativeWindow =
        static_cast<GLFWwindow*>(m_window->getNativeWindow());

    glfwSwapBuffers(nativeWindow);
}

void OpenGLRenderer::onWindowResize(uint32_t width, uint32_t height) {
    glViewport(0, 0, width, height);
}

void OpenGLRenderer::drawIndexed() {
    m_shader->bind();
    m_vertexArray->bind();

    glDrawElements(
        GL_TRIANGLES,
        m_indexBuffer->getCount(),
        GL_UNSIGNED_INT,
        nullptr
    );
}

void OpenGLRenderer::bindPipeline() {

}

void OpenGLRenderer::updateUniformBuffer() {
    static auto startTime = std::chrono::high_resolution_clock::now();

    auto currentTime = std::chrono::high_resolution_clock::now();
    float time = std::chrono::duration<float, std::chrono::seconds::period>(
        currentTime - startTime
    ).count();

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, time * glm::radians(90.0f), glm::vec3(1.0f, 0.0f, 0.0f));
    model = glm::rotate(model, time * glm::radians(90.0f), glm::vec3(0.0f, 1.0f, 0.0f));
    model = glm::rotate(model, time * glm::radians(90.0f), glm::vec3(0.0f, 0.0f, 1.0f));

    UniformBufferObject ubo{};
    ubo.model = model;
    ubo.view = glm::lookAt(
        glm::vec3(2.0f, 2.0f, 2.0f),
        glm::vec3(0.0f, 0.0f, 0.0f),
        glm::vec3(0.0f, 0.0f, 1.0f)
    );

    float aspect =
        static_cast<float>(m_window->getWidth()) /
        static_cast<float>(m_window->getHeight());

    ubo.proj = glm::perspective(
        glm::radians(45.0f),
        aspect,
        0.1f,
        10.0f
    );

    // Vulkan only:
    // ubo.proj[1][1] *= -1;

    m_uniformBuffer->setData(&ubo, sizeof(ubo));
}

}
