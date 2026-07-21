#version 460
#extension GL_EXT_ray_tracing : require

layout(location = 0) rayPayloadInEXT vec3 hitValue;
hitAttributeEXT vec2 attribs;

void main() {
    // Solid color on hit — proves the ray actually intersected geometry.
    // Barycentric-based color makes it easy to visually confirm triangle shape too.
    const vec3 barycentricColor = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);
    hitValue = barycentricColor;
}
