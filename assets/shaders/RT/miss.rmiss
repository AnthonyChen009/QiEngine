#version 460
#extension GL_EXT_ray_tracing : require

layout(location = 0) rayPayloadInEXT vec3 hitValue;

void main() {
    // Sky color placeholder — solid so a miss is visually distinct from a hit
    hitValue = vec3(0.1, 0.2, 0.4);
}
