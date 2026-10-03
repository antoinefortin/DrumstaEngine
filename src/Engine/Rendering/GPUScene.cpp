#include "GPUScene.h"
#include <cmath>
#include <algorithm>

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
    const std::vector<DrawArraysIndirectCommand>& commands,
    //const std::vector<GLuint64>& textureHandles
    const std::vector<ImageData>& images
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

    // SSBO Texture
    UploadTextures(images);

}


void GPUScene::UploadTextures(const std::vector<ImageData>& images)
{
    glTextures.resize(images.size());
    textureHandles.resize(images.size());
    
    for (size_t i{}; i < images.size(); i++)
    {
        const ImageData& img = images[i];

        GLsizei levels = 1 + (GLsizei)std::floor(std::log2((float)std::max(img.width, img.height)));

        GLuint tex;
        glCreateTextures(GL_TEXTURE_2D, 1, &tex);
        glTextureStorage2D(tex, levels, GL_SRGB8_ALPHA8, img.width, img.height);
        glTextureSubImage2D(tex, 0, 0, 0, img.width, img.height,
            GL_RGBA, GL_UNSIGNED_BYTE, img.rgba.data());
        glGenerateTextureMipmap(tex);

        glTextureParameteri(tex, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTextureParameteri(tex, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTextureParameteri(tex, GL_TEXTURE_WRAP_S, GL_REPEAT);
        glTextureParameteri(tex, GL_TEXTURE_WRAP_T, GL_REPEAT);

        GLuint64 handle = glGetTextureHandleARB(tex);
        glMakeTextureHandleResidentARB(handle);

        glTextures[i] = tex;
        textureHandles[i] = handle;
    }


    std::vector<GLuint64> handles = textureHandles;
    if (handles.empty())
    {
        handles.push_back(0);
    }

    glCreateBuffers(1, &ssboTextureHandles);
    glNamedBufferStorage(ssboTextureHandles, handles.size() * sizeof(GLuint64),
        handles.data(), 0);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 5, ssboTextureHandles);

}