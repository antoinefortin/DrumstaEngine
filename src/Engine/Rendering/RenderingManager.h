#pragma once

#include "Passes/OpaquePass.h"

#include <glad/glad.h>
#include <glm/glm.hpp>




class GPUScene;
class Texture;

class RenderingManager
{
public:
    RenderingManager();
    void Render(
        const glm::mat4& viewProj,
        Texture& texture,
        const GPUScene& gpuScene,
        GLuint shaderProgram
    );


private:
    OpaquePass opaquePass;
};
