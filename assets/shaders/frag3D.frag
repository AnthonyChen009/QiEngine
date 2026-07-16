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

layout(binding = 1) uniform sampler2D texSampler[1024];
layout(location = 0) in vec4 fragColor;
layout(location = 1) in vec2 fragTexCoord;
layout(location = 2) in vec3 fragNormal;
layout(location = 0) out vec4 outColor;

void main() {
    vec4 albedo;
    if (push.textureIndex == 0xFFFFFFFF)
        albedo = fragColor;
    else
        albedo = texture(texSampler[push.textureIndex], fragTexCoord) * fragColor;

    vec3 N = normalize(fragNormal);
    vec3 L = normalize(-ubo.lightDirection);

    float diff = max(dot(N, L), 0.0);
    vec3 diffuse = diff * ubo.lightColor * ubo.lightIntensity;
    vec3 ambient = ubo.ambientColor * ubo.ambientIntensity;

    vec3 lighting = ambient + diffuse;
    outColor = vec4(albedo.rgb * lighting, albedo.a);
}
