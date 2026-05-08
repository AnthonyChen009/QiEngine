#pragma once
#include "RendererBackend.hpp"
#include "Window.hpp"
#include <optional>
#include "OpenGLVertexBuffer.hpp"
#include "OpenGLIndexBuffer.hpp"
#include "OpenGLVertexArray.hpp"
#include "OpenGLShader.hpp"
#include "OpenGLUniformBuffer.hpp"

namespace Qi {

class OpenGLRenderer : public RendererBackend {
public:
    void init(Window& window) override;
    void shutdown() override;

    bool beginFrame() override;
    void endFrame() override;

    void onWindowResize(uint32_t width, uint32_t height) override;
public:
    void drawIndexed(uint32_t count) override;
    void updateUniformBuffer() override;
    void bindPipeline() override;
    void pushConstants(const PushConstant2D& push) override;
private:
    Window* m_window = nullptr;
    std::optional<OpenGLVertexArray> m_vertexArray;
    std::optional<OpenGLVertexBuffer> m_vertexBuffer;
    std::optional<OpenGLIndexBuffer> m_indexBuffer;
    std::optional<OpenGLShader> m_shader;
    std::optional<OpenGLUniformBuffer> m_uniformBuffer;
    std::optional<OpenGLUniformBuffer> m_pushConstantBuffer;
};

}
