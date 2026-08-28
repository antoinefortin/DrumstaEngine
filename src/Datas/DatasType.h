#pragma once

#include <vector>


struct Vertex
{
    float x, y, z;
    float nx, ny, nz;
};

struct Mesh
{
    std::vector<Vertex>   verts;
    std::vector<uint32_t> indices;
    uint32_t              materialId = 0;

    uint32_t vertexCount() const { return (uint32_t)verts.size(); }
    uint32_t indexCount()  const { return (uint32_t)indices.size(); }


};


struct DrawColor
{
    float red;
    float green;
    float blue;
    float padding1;

};
