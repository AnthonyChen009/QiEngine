#version 450

layout(push_constant) uniform PushConstants {
    mat4 transform;
    vec4 color;
    uint textureIndex;
} push;

layout(set = 0, binding = 0) uniform UniformBufferObject {
    mat4 view;
    mat4 proj;

    vec3 lightDirection;
    vec3 lightColor;
    float lightIntensity;

    vec3 ambientColor;
    float ambientIntensity;
} ubo;

layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inNormal;
layout(location = 2) in vec3 inColor;
layout(location = 3) in vec2 inTexCoord;

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 fragTexCoord;
layout(location = 2) out vec3 fragNormal;

void main() {
    gl_Position = ubo.proj * ubo.view * push.transform * vec4(inPosition, 1.0);

    fragColor = push.color;
    fragTexCoord = inTexCoord;
    fragNormal = mat3(transpose(inverse(push.transform))) * inNormal;
}
