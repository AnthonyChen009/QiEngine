#version 460
#extension GL_EXT_ray_tracing : require

struct HitPayload {
    float hitDistance;
    vec3 worldNormal;
    int objectIndex;
    int hitType;
    bool isBackface;
    vec3 albedo;
    uint seed;
    vec3 emissionColor;
    float emissionPower;
    vec3 nextRayDirection;
};

layout(location = 0) rayPayloadInEXT HitPayload payload;

void main() {
    payload.hitDistance = -1.0;
    payload.hitType = 0;
}
