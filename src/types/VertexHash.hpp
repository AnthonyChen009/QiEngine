#pragma once

#include "types/Vertex.hpp"
namespace Qi {

struct VertexHash {
    size_t operator()(const Vertex& v) const {
        size_t seed = 0;
        auto hashCombine = [&seed](float f) {
            size_t h = std::hash<float>()(f);
            seed ^= h + 0x9e3779b9 + (seed << 6) + (seed >> 2);
        };
        hashCombine(v.pos.x); hashCombine(v.pos.y); hashCombine(v.pos.z);
        hashCombine(v.normal.x); hashCombine(v.normal.y); hashCombine(v.normal.z);
        hashCombine(v.color.x); hashCombine(v.color.y); hashCombine(v.color.z);
        hashCombine(v.texCoord.x); hashCombine(v.texCoord.y);
        return seed;
    }
};

}
