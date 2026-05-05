#pragma once
#include "RendererBackend.hpp"
#include "Window.hpp"
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
    void drawIndexed() override;
    void updateUniformBuffer() override;
    void bindPipeline() override;
private:
    Window* m_window = nullptr;
    std::unique_ptr<OpenGLVertexArray> m_vertexArray;
    std::unique_ptr<OpenGLVertexBuffer> m_vertexBuffer;
    std::unique_ptr<OpenGLIndexBuffer> m_indexBuffer;
    std::unique_ptr<OpenGLShader> m_shader;
    std::unique_ptr<OpenGLUniformBuffer> m_uniformBuffer;
};

}
