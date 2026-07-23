#version 460
#extension GL_EXT_ray_tracing : require

struct HitPayload {
    float hitDistance;
    vec3 worldNormal;
    int objectIndex;
    int hitType;
    bool isBackface;
    int bounceCount;
    vec3 albedo;
    uint seed;
    vec3 accumulatedLight;
    vec3 throughput;
};

layout(location = 0) rayPayloadInEXT HitPayload payload;

void main() {
    payload.hitDistance = -1.0;
    payload.hitType = 0;
    payload.accumulatedLight += vec3(0.1, 0.2, 0.4) * payload.throughput;
}
