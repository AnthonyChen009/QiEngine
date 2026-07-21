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
layout(binding = 2) uniform sampler2D rtOutput;

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
    vec3 rasterColor = albedo.rgb * lighting;

    // RT debug pass-through: screen-space UV into the RT output buffer.
    // TODO: replace with real hybrid blending (shadow/AO term) later.
    vec2 screenUV = gl_FragCoord.xy / vec2(textureSize(rtOutput, 0));
    vec3 rtColor = texture(rtOutput, screenUV).rgb;

    outColor = vec4(rtColor, albedo.a);
    // outColor = vec4(rasterColor, albedo.a); // <- previous rasterized-only output, kept for reference
}
