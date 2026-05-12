#include "OpenGLShader.hpp"

#include "core/Assert.hpp"
#include "core/Log.hpp"

#include <glad/glad.h>

namespace Qi {

OpenGLShader::OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc) {
    uint32_t vertexShader = compileShader(GL_VERTEX_SHADER, vertexSrc);
    uint32_t fragmentShader = compileShader(GL_FRAGMENT_SHADER, fragmentSrc);

    m_rendererID = glCreateProgram();

    glAttachShader(m_rendererID, vertexShader);
    glAttachShader(m_rendererID, fragmentShader);
    glLinkProgram(m_rendererID);

    int success = 0;
    glGetProgramiv(m_rendererID, GL_LINK_STATUS, &success);

    if (!success) {
        char infoLog[1024];
        glGetProgramInfoLog(m_rendererID, sizeof(infoLog), nullptr, infoLog);

        QI_CORE_ERROR("OpenGL shader link failed: {0}", infoLog);
        QI_CORE_ASSERT(false, "Failed to link OpenGL shader program!");
    }

    glDetachShader(m_rendererID, vertexShader);
    glDetachShader(m_rendererID, fragmentShader);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

OpenGLShader::~OpenGLShader() {
    if (m_rendererID)
        glDeleteProgram(m_rendererID);
}

void OpenGLShader::bind() const {
    glUseProgram(m_rendererID);
}

void OpenGLShader::unbind() const {
    glUseProgram(0);
}

uint32_t OpenGLShader::compileShader(uint32_t type, const std::string& source) {
    uint32_t shader = glCreateShader(type);

    const char* src = source.c_str();
    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    int success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success) {
        char infoLog[1024];
        glGetShaderInfoLog(shader, sizeof(infoLog), nullptr, infoLog);

        QI_CORE_ERROR("OpenGL shader compile failed: {0}", infoLog);
        QI_CORE_ASSERT(false, "Failed to compile OpenGL shader!");
    }

    return shader;
}

}
