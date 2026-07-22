#version 460
#extension GL_EXT_ray_tracing : require

struct HitPayload {
    float hitDistance;
    vec3 worldPosition;
    vec3 worldNormal;
    int objectIndex;
    int hitType;
    bool isBackface;
};

layout(location = 0) rayPayloadInEXT HitPayload payload;
hitAttributeEXT vec2 attribs;

void main() {
    payload.hitDistance = gl_HitTEXT;
    payload.worldPosition = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;
    payload.worldNormal = vec3(0.0); // real normal fetch is future work — see note below
    payload.objectIndex = gl_InstanceCustomIndexEXT;
    payload.hitType = 1; // 1 = hit
    payload.isBackface = false; // needs real normal to compute properly — placeholder
}
