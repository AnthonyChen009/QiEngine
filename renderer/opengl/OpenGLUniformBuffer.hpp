#pragma once

#include <cstdint>

namespace Qi {

class OpenGLUniformBuffer {
public:
    OpenGLUniformBuffer(uint32_t size, uint32_t binding);
    ~OpenGLUniformBuffer();

    void setData(const void* data, uint32_t size, uint32_t offset = 0);

private:
    uint32_t m_rendererID = 0;
};

}
