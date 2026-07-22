#version 460
#extension GL_EXT_ray_tracing : require
#extension GL_EXT_buffer_reference2 : require
#extension GL_EXT_scalar_block_layout : require
#extension GL_EXT_shader_explicit_arithmetic_types_int64 : require

struct HitPayload {
    float hitDistance;
    vec3 worldNormal;
    int objectIndex;
    int hitType;
    bool isBackface;
    int bounceCount;
};

struct Vertex {
    vec3 pos;
    vec3 normal;
    vec3 color;
    vec2 texCoord;
};

struct InstanceAddresses {
    uint64_t vertexBufferAddress;
    uint64_t indexBufferAddress;
};

layout(buffer_reference, scalar) readonly buffer VertexBuffer { Vertex v[]; };
layout(buffer_reference, scalar) readonly buffer IndexBuffer { uint i[]; };
layout(binding = 0, set = 0) uniform accelerationStructureEXT topLevelAS;
layout(binding = 3, set = 0, scalar) readonly buffer InstanceAddressesBuffer { InstanceAddresses addrs[]; };

layout(location = 0) rayPayloadInEXT HitPayload payload;
layout(location = 1) rayPayloadEXT HitPayload bouncePayload;
hitAttributeEXT vec2 attribs;

void main() {
    InstanceAddresses addr = addrs[gl_InstanceCustomIndexEXT];
    VertexBuffer vb = VertexBuffer(addr.vertexBufferAddress);
    IndexBuffer ib = IndexBuffer(addr.indexBufferAddress);

    uint i0 = ib.i[3 * gl_PrimitiveID + 0];
    uint i1 = ib.i[3 * gl_PrimitiveID + 1];
    uint i2 = ib.i[3 * gl_PrimitiveID + 2];

    Vertex v0 = vb.v[i0];
    Vertex v1 = vb.v[i1];
    Vertex v2 = vb.v[i2];

    vec3 barycentrics = vec3(1.0 - attribs.x - attribs.y, attribs.x, attribs.y);
    vec3 localNormal = v0.normal * barycentrics.x + v1.normal * barycentrics.y + v2.normal * barycentrics.z;
    vec3 worldNormal = normalize(vec3(localNormal * mat3(gl_WorldToObjectEXT)));

    vec3 hitWorldPosition = gl_WorldRayOriginEXT + gl_WorldRayDirectionEXT * gl_HitTEXT;

    payload.hitDistance = gl_HitTEXT;
    payload.worldNormal = worldNormal;
    payload.objectIndex = gl_InstanceCustomIndexEXT;
    payload.hitType = 1;
    payload.isBackface = dot(gl_WorldRayDirectionEXT, worldNormal) > 0.0;

    if (payload.bounceCount < 1) {
        vec3 reflectDir = reflect(gl_WorldRayDirectionEXT, worldNormal);
        bouncePayload.bounceCount = payload.bounceCount + 1;
        traceRayEXT(
            topLevelAS, gl_RayFlagsOpaqueEXT, 0xff, 0, 0, 0,
            hitWorldPosition + worldNormal * 0.001,
            0.001, reflectDir, 10000.0, 1
        );
        // hadBounceHit removed — no longer tracked; bouncePayload.hitType
        // is still available here if you want to use it directly for
        // real reflected-color shading later, just not stored back into `payload`.
    }
}
