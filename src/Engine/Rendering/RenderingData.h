#pragma once
#include <cstdint>



struct DrawArraysIndirectCommand
{
    uint32_t count;
    uint32_t instanceCount;
    uint32_t first;
    uint32_t baseInstance;
};

struct DrawMetadata
{
    uint32_t baseVertex;
    uint32_t materialIndex;
    uint32_t padding0;
    uint32_t padding1;
};

