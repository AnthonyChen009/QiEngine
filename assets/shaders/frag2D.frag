#version 450

layout(push_constant) uniform PushConstants {
    mat4 transform;
    vec4 color;
    uint textureIndex;
} push;

layout(binding = 1) uniform sampler2D texSampler[1024];

layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;

layout(location = 0) out vec4 outColor;

void main() {
    if (push.textureIndex == 0xFFFFFFFF)
        outColor = fragColor;
    else
        outColor = texture(texSampler[push.textureIndex], fragTexCoord) * fragColor;
}
