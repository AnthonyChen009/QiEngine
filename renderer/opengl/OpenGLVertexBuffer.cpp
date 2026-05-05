#include "OpenGLVertexBuffer.hpp"

#include <glad/glad.h>

namespace Qi {

OpenGLVertexBuffer::OpenGLVertexBuffer(const void* data, uint32_t size) {
    glGenBuffers(1, &m_rendererID);
    glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

OpenGLVertexBuffer::~OpenGLVertexBuffer() {
    if (m_rendererID)
        glDeleteBuffers(1, &m_rendererID);
}

void OpenGLVertexBuffer::bind() const {
    glBindBuffer(GL_ARRAY_BUFFER, m_rendererID);
}

void OpenGLVertexBuffer::unbind() const {
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}

}
