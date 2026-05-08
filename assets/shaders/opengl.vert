#version 460 core

layout(std140, binding = 0) uniform UniformBufferObject {
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

layout(std140, binding = 1) uniform PushConstants {
    mat4 transform;
    vec4 color;
} push;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;

layout(location = 0) out vec4 fragColor;

void main() {
    gl_Position = ubo.proj * ubo.view * push.transform * vec4(inPosition, 1.0);
    fragColor = push.color;
}
