#include "GPUScene.h"



/*

    const GLuint buffers[] = {
        ssboVertex,
        ssboIndex,
        ssboTransform,
        ssboMetadata,
        ssboColor,
        indirectBuffer
    };

    glDeleteBuffers(6, buffers);
    glDeleteVertexArrays(1, &vao);
*/

GPUScene::~GPUScene()
{


    const GLuint buffersToDelete[] =
    {
        ssboVertex,
        ssboIndex,
        ssboTransform,
        ssboMetadata,
        ssboColor,
        indirectBuffer
    };


    glDeleteBuffers(6, buffersToDelete);
    glDeleteVertexArrays(1, &vao);
}

// bind ssbo
void GPUScene::Bind() const
{

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboVertex);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboIndex);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssboTransform);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssboMetadata);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssboColor);

    glBindVertexArray(vao);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
}

// drae scene from upolaoded gpu datas 
void GPUScene::Draw() const
{

    glMultiDrawArraysIndirect(
        GL_TRIANGLES,
        nullptr,
        drawCount,
        0
    );
}



void GPUScene::Upload(
    const std::vector<Vertex>& vertices,
    const std::vector<uint32_t>& indices,
    const std::vector<glm::mat4>& transforms,
    const std::vector<DrawMetadata>& metadata,
    const std::vector<DrawColor>& colors,
    const std::vector<DrawArraysIndirectCommand>& commands
)
{
    glGenVertexArrays(1, &vao);
    // Vertex SBO
    glGenBuffers(1, &ssboVertex);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboVertex);
    glBufferData(GL_SHADER_STORAGE_BUFFER, vertices.size() * sizeof(Vertex), vertices.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboVertex);

    // Indinces 
    glGenBuffers(1, &ssboIndex);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboIndex);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        indices.size() * sizeof(uint32_t),
        indices.data(),
        GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboIndex);

    // Transfdorm
    glGenBuffers(1, &ssboTransform);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboTransform);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        transforms.size() * sizeof(glm::mat4), 
        transforms.data(),
        GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssboTransform);

    // Metadata
    glGenBuffers(1, &ssboMetadata);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboMetadata);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        metadata.size() * sizeof(DrawMetadata),
        metadata.data(),
        GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssboMetadata);

    glGenBuffers(1, &ssboColor);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboColor);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        colors.size() * sizeof(DrawColor),
        colors.data(),
        GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssboColor);

    // GPU draw command
    glGenBuffers(1, &indirectBuffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
    glBufferData(GL_DRAW_INDIRECT_BUFFER,
        commands.size() * sizeof(DrawArraysIndirectCommand),
        commands.data(),
        GL_DYNAMIC_DRAW);

    // draew count GPOU side 
    drawCount = static_cast<GLsizei>(commands.size());
}
