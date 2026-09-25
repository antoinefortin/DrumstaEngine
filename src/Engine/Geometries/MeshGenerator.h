#pragma once
#include "../../Datas/DatasType.h"
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cmath>
#include <map>
#include <algorithm>

Mesh generateCube(float size = 1.0f)
{
    Mesh mesh;
    float h = size * 0.5f;

    mesh.verts = {
        { -h, -h, -h },
        {  h, -h, -h },
        {  h,  h, -h },
        { -h,  h, -h },
        { -h, -h,  h },
        {  h, -h,  h },
        {  h,  h,  h },
        { -h,  h,  h },
    };

    mesh.indices = {
        0, 1, 2,  2, 3, 0,
        4, 6, 5,  6, 4, 7,
        0, 3, 7,  7, 4, 0,
        1, 5, 6,  6, 2, 1,
        0, 4, 5,  5, 1, 0,
        3, 2, 6,  6, 7, 3,
    };

    return mesh;
}
Mesh generateIcosphere(float radius = 1.0f, int subdivisions = 1)
{
    // Al;gos stolen from Belnder src code
    Mesh mesh;
    const float t = (1.0f + std::sqrt(5.0f)) * 0.5f;
    std::vector<Vertex> baseVerts = {
        {-1,  t,  0}, { 1,  t,  0}, {-1, -t,  0}, { 1, -t,  0},
        { 0, -1,  t}, { 0,  1,  t}, { 0, -1, -t}, { 0,  1, -t},
        { t,  0, -1}, { t,  0,  1}, {-t,  0, -1}, {-t,  0,  1},
    };
    auto normalize = [](Vertex v) -> Vertex {
        float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        return { v.x / len, v.y / len, v.z / len };
        };

    for (auto& v : baseVerts)
        v = normalize(v);

    mesh.verts = baseVerts;


    std::vector<uint32_t> baseIndices = {
        0, 11, 5,   0, 5, 1,    0, 1, 7,    0, 7, 10,   0, 10, 11,
        1, 5, 9,    5, 11, 4,   11, 10, 2,  10, 7, 6,   7, 1, 8,
        3, 9, 4,    3, 4, 2,    3, 2, 6,    3, 6, 8,    3, 8, 9,
        4, 9, 5,    2, 4, 11,   6, 2, 10,   8, 6, 7,    9, 8, 1,
    };

    mesh.indices = baseIndices;
    std::map<uint64_t, uint32_t> midpointCache;

    auto getMidpoint = [&](uint32_t i0, uint32_t i1) -> uint32_t
        {
            uint64_t key = (std::min(i0, i1) < std::max(i0, i1))
                ? ((uint64_t)std::min(i0, i1) << 32) | std::max(i0, i1)
                : ((uint64_t)std::max(i0, i1) << 32) | std::min(i0, i1);

            auto it = midpointCache.find(key);
            if (it != midpointCache.end())
                return it->second;

            Vertex v0 = mesh.verts[i0];
            Vertex v1 = mesh.verts[i1];
            Vertex mid = {
                (v0.x + v1.x) * 0.5f,
                (v0.y + v1.y) * 0.5f,
                (v0.z + v1.z) * 0.5f
            };
            mid = normalize(mid);

            uint32_t newIndex = (uint32_t)mesh.verts.size();
            mesh.verts.push_back(mid);
            midpointCache[key] = newIndex;
            return newIndex;
        };

    for (int s = 0; s < subdivisions; ++s)
    {
        std::vector<uint32_t> newIndices;
        newIndices.reserve(mesh.indices.size() * 4);

        for (size_t i = 0; i < mesh.indices.size(); i += 3)
        {
            uint32_t i0 = mesh.indices[i];
            uint32_t i1 = mesh.indices[i + 1];
            uint32_t i2 = mesh.indices[i + 2];

            uint32_t a = getMidpoint(i0, i1);
            uint32_t b = getMidpoint(i1, i2);
            uint32_t c = getMidpoint(i2, i0);

            newIndices.insert(newIndices.end(), {
                i0, a, c,
                i1, b, a,
                i2, c, b,
                a,  b, c
                });
        }

        mesh.indices = newIndices;
        midpointCache.clear();
    }

    for (auto& v : mesh.verts)
    {
        v.x *= radius;
        v.y *= radius;
        v.z *= radius;
    }

    return mesh;
}
