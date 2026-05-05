#include "OpenGLVertexArray.hpp"

#include <glad/glad.h>

namespace Qi {

OpenGLVertexArray::OpenGLVertexArray() {
    glGenVertexArrays(1, &m_rendererID);
}

OpenGLVertexArray::~OpenGLVertexArray() {
    if (m_rendererID)
        glDeleteVertexArrays(1, &m_rendererID);
}

void OpenGLVertexArray::bind() const {
    glBindVertexArray(m_rendererID);
}

void OpenGLVertexArray::unbind() const {
    glBindVertexArray(0);
}

}
