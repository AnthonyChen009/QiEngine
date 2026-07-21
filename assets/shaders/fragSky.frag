#version 450
layout(location = 0) in vec3 inViewDir;
layout(location = 0) out vec4 outColor;
void main() {
    vec3 horizonColor = vec3(0.65, 0.80, 1.0);
    vec3 zenithColor  = vec3(0.25, 0.50, 0.95);
    float t = clamp(inViewDir.y * 0.5 + 0.5, 0.0, 1.0);
    vec3 sky = mix(horizonColor, zenithColor, t);
    outColor = vec4(sky, 1.0);
}
