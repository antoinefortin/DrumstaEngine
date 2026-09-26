#include "RenderingManager.h"
#include "GPUScene.h"
#include "OpenGL/Texture.h"

#include <glm/gtc/type_ptr.hpp>


RenderingManager::RenderingManager()
{

}
void RenderingManager::Render(
    const glm::mat4& viewProj,
    Texture& texture,
    const GPUScene& gpuScene,
    GLuint shaderProgram)
{

    opaquePass.Execute(
        viewProj,
        texture,
        gpuScene,
        shaderProgram
    );
}
