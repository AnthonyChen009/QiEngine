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
    vec3 albedo;
    uint seed;
    vec3 accumulatedLight;
    vec3 throughput;
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
    uint materialIndex;
};

struct GPUMaterial {
    vec3 albedo;
    float roughness;
    float metallic;
    vec3 emissionColor;
    float emissionPower;
    float specularProbability;
    uint isGlass;
    float ior;
    vec3 absorption;
    float absorptionStrength;
};

uint pcgHash(uint seed) {
    uint state = seed * 747796405u + 2891336453u;
    uint word = ((state >> ((state >> 28u) + 4u)) ^ state) * 277803737u;
    return (word >> 22u) ^ word;
}

float randomFloat(inout uint seed) {
    seed = pcgHash(seed);
    return float(seed) / 4294967295.0;
}

vec3 vectorInUnitSphere(inout uint seed) {
    return normalize(vec3(
        randomFloat(seed) * 2.0 - 1.0,
        randomFloat(seed) * 2.0 - 1.0,
        randomFloat(seed) * 2.0 - 1.0
    ));
}

vec3 randomOnHemisphere(inout uint seed, vec3 normal) {
    vec3 onUnitSphere = vectorInUnitSphere(seed);
    return (dot(onUnitSphere, normal) > 0.0) ? onUnitSphere : -onUnitSphere;
}

vec3 randomVec3(inout uint seed) {
    return vec3(
        randomFloat(seed) - 0.5,
        randomFloat(seed) - 0.5,
        randomFloat(seed) - 0.5
    );
}

float randomValueNormalDistribution(inout uint seed) {
    float theta = 2.0 * 3.14159265 * randomFloat(seed);
    float rho = sqrt(-2.0 * log(randomFloat(seed)));
    return rho * cos(theta);
}


vec3 randomDirection(inout uint seed) {
    float x = randomValueNormalDistribution(seed);
	float y = randomValueNormalDistribution(seed);
	float z = randomValueNormalDistribution(seed);
    return normalize(vec3(x, y, z));
}


layout(buffer_reference, scalar) readonly buffer VertexBuffer { Vertex v[]; };
layout(buffer_reference, scalar) readonly buffer IndexBuffer { uint i[]; };
layout(binding = 0, set = 0) uniform accelerationStructureEXT topLevelAS;
layout(binding = 3, set = 0, scalar) readonly buffer InstanceAddressesBuffer { InstanceAddresses addrs[]; };
layout(binding = 4, set = 0, scalar) readonly buffer MaterialsBuffer { GPUMaterial materials[]; };

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

    GPUMaterial mat = materials[addr.materialIndex];

    payload.hitDistance = gl_HitTEXT;
    payload.worldNormal = worldNormal;
    payload.objectIndex = gl_InstanceCustomIndexEXT;
    payload.hitType = 1;
    payload.isBackface = dot(gl_WorldRayDirectionEXT, worldNormal) > 0.0;
    //actual light calcs
    bool isSpecularBounce = mat.specularProbability >= randomFloat(payload.seed);
    vec3 diffuseDir = normalize(payload.worldNormal + randomDirection(payload.seed));
    vec3 specularDir = reflect(gl_WorldRayDirectionEXT, payload.worldNormal);

    vec3 emittedLight = mat.emissionColor * mat.emissionPower;
    payload.accumulatedLight += emittedLight * payload.throughput;
    payload.throughput *= mat.albedo;
    const int maxBounces = 4;
    if (payload.bounceCount < maxBounces) {
        bool shouldContinue = true;

        if (payload.bounceCount >= 2) {
            float p = clamp(max(payload.throughput.r, max(payload.throughput.g, payload.throughput.b)), 0.1, 1.0);
            if (randomFloat(payload.seed) > p) {
                shouldContinue = false;
            } else {
                payload.throughput /= p;
            }
        }

        if (shouldContinue) {
            vec3 reflectDir = normalize(mix(diffuseDir, specularDir, (1.0 - mat.roughness) * float(isSpecularBounce)));
            bouncePayload.bounceCount = payload.bounceCount + 1;
            bouncePayload.seed = payload.seed; // already advanced by randomFloat call above
            bouncePayload.accumulatedLight = payload.accumulatedLight;
            bouncePayload.throughput = payload.throughput;
            traceRayEXT(
                topLevelAS, gl_RayFlagsOpaqueEXT, 0xff, 0, 0, 0,
                hitWorldPosition + worldNormal * 0.001,
                0.001, reflectDir, 10000.0, 1
            );
            payload.accumulatedLight = bouncePayload.accumulatedLight;
        }
    }
}
