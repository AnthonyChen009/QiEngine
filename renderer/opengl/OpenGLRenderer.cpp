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

    m_vertexArray.emplace();
    m_vertexArray->bind();
    m_vertexBuffer.emplace(vertices.data(), vertices.size() * sizeof(Vertex));
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

    m_indexBuffer.emplace(indices.data(), static_cast<uint32_t>(indices.size()));
    m_indexBuffer->bind();
    m_vertexArray->unbind();

    std::string vertexSrc = FileSystem::readTextFile("shaders/opengl.vert");
    std::string fragmentSrc = FileSystem::readTextFile("shaders/opengl.frag");

    m_shader.emplace(vertexSrc, fragmentSrc);
    m_uniformBuffer.emplace(sizeof(UniformBufferObject), 0);
    m_pushConstantBuffer.emplace(sizeof(PushConstant2D), 1);

    QI_CORE_INFO("OpenGL initialized");
    QI_CORE_INFO("OpenGL Vendor: {0}", reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
    QI_CORE_INFO("OpenGL Renderer: {0}", reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
    QI_CORE_INFO("OpenGL Version: {0}", reinterpret_cast<const char*>(glGetString(GL_VERSION)));
}

void OpenGLRenderer::shutdown() {
    m_window = nullptr;

    QI_CORE_INFO("OpenGLRenderer shutdown");
}

bool OpenGLRenderer::beginFrame() {
    glClearColor(0.0f, 0.0f, 0.0f, 0.0f);
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

void OpenGLRenderer::drawIndexed(uint32_t count) {
    glDrawElements(
        GL_TRIANGLES,
        m_indexBuffer->getCount(),
        GL_UNSIGNED_INT,
        nullptr
    );
}

void OpenGLRenderer::bindPipeline() {
    m_shader->bind();
    m_vertexArray->bind();
}

void OpenGLRenderer::pushConstants(const PushConstant2D& push) {
    m_pushConstantBuffer->setData(&push, sizeof(PushConstant2D));
}

void OpenGLRenderer::updateUniformBuffer() {
    UniformBufferObject ubo{};
    ubo.model = glm::mat4(1.0f);
    ubo.view = glm::mat4(1.0f);
    float width = static_cast<float>(m_window->getWidth());
    float height = static_cast<float>(m_window->getHeight());
    ubo.proj = glm::ortho(-width/2.0f, width/2.0f, -height/2.0f, height/2.0f, -1.0f, 1.0f);
    m_uniformBuffer->setData(&ubo, sizeof(ubo));
}

}
