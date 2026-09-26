#pragma once

#include <vector>
#include <glm/glm.hpp>
///  INPUT 
enum KeyboardMode
{
    WASD = 0,
    QWASED,
    WASD_ARROW,
};
/// INPUT


/// Rendering
struct CPU_Color
{
    float r, g, b;
};

struct Vertex
{
    float x, y, z;
    float nx, ny, nz;
    float u, v;
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

struct ImageData
{
    int width = 0;
    int height = 0;

    std::vector<unsigned char> rgba;
};

struct MaterialData
{
    glm::vec4 baseColorFactor{ 1.0f };
    int32_t baseColorTextureIndex = -1;
};
/// Rendering
