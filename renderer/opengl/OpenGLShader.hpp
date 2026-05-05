#pragma once

#include <cstdint>
#include <string>

namespace Qi {

class OpenGLShader {
public:
    OpenGLShader(const std::string& vertexSrc, const std::string& fragmentSrc);
    ~OpenGLShader();

    void bind() const;
    void unbind() const;

private:
    uint32_t compileShader(uint32_t type, const std::string& source);

private:
    uint32_t m_rendererID = 0;
};

}
