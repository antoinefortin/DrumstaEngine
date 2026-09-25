#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>


class GPUScene;
class Texture;

class OpaquePass
{
public:
    OpaquePass();
    void Execute(
        const glm::mat4& viewProj,
        Texture& texture,
        const GPUScene& gpuScene,
        GLuint shaderProgram
    );



private:

};
