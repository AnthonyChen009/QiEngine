#version 450
layout(location = 0) out vec3 outViewDir;

layout(set = 0, binding = 0) uniform SkyUBO {
    mat4 invViewProj;
} ubo;

void main() {
    vec2 positions[3] = vec2[](
        vec2(-1.0, -1.0),
        vec2( 3.0, -1.0),
        vec2(-1.0,  3.0)
    );
    vec2 pos = positions[gl_VertexIndex];
    gl_Position = vec4(pos, 0.9999, 1.0);

    vec4 worldPos = ubo.invViewProj * vec4(pos, 1.0, 1.0);
    outViewDir = normalize(worldPos.xyz / worldPos.w);
}
