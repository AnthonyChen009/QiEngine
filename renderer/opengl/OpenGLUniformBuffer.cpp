#include "OpenGLUniformBuffer.hpp"

#include <glad/glad.h>

namespace Qi {

OpenGLUniformBuffer::OpenGLUniformBuffer(uint32_t size, uint32_t binding) {
    glGenBuffers(1, &m_rendererID);
    glBindBuffer(GL_UNIFORM_BUFFER, m_rendererID);
    glBufferData(GL_UNIFORM_BUFFER, size, nullptr, GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_UNIFORM_BUFFER, binding, m_rendererID);
}

OpenGLUniformBuffer::~OpenGLUniformBuffer() {
    if (m_rendererID)
        glDeleteBuffers(1, &m_rendererID);
}

void OpenGLUniformBuffer::setData(const void* data, uint32_t size, uint32_t offset) {
    glBindBuffer(GL_UNIFORM_BUFFER, m_rendererID);
    glBufferSubData(GL_UNIFORM_BUFFER, offset, size, data);
}

}
