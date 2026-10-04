#include "OpaquePass.h"
#include "../GPUScene.h"
#include "../OpenGL/Texture.h"

#include <glm/gtc/type_ptr.hpp>



OpaquePass::OpaquePass()
{

}

void OpaquePass::Execute(
    const glm::mat4& viewProj,
    //Texture& texture,
    const GPUScene& gpuScene,
    GLuint shaderProgram

)
{

    glEnable(GL_DEPTH_TEST);
    glDepthMask(GL_TRUE);
    glDisable(GL_BLEND);

    glUseProgram(shaderProgram);

    /*
    
    
    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, texture.getGPUHandle());
    GLint locTexture = glGetUniformLocation(shaderProgram, "uTexture");
    glUniform1i(locTexture, 0);
    */
    GLint locViewProj = glGetUniformLocation(shaderProgram, "viewProj");
    glUniformMatrix4fv(locViewProj, 1, GL_FALSE, glm::value_ptr(viewProj));

    GLint locLightDir = glGetUniformLocation(shaderProgram, "lightDir");
    glUniform3f(locLightDir, 0.4f, 0.8f, 0.3f);

    GLint locLightColor = glGetUniformLocation(shaderProgram, "lightColor");
    glUniform3f(locLightColor, 1.0f, 1.0f, 1.0f);

    GLint locBaseColor = glGetUniformLocation(shaderProgram, "baseColor");
    glUniform3f(locBaseColor, 0.8f, 0.8f, 0.8f);

    //    glBindVertexArray(vao);

    gpuScene.Bind();
    if (gpuScene.UsesBindless())
    {
        gpuScene.Draw();
        return;
    }

    const GLint samplerLoc = glGetUniformLocation(shaderProgram, "uBaseColor");
    const GLint hasTextureLoc = glGetUniformLocation(shaderProgram, "uHasTexture");
    glUniform1i(samplerLoc, 0);

    for (GLsizei draw = 0; draw < gpuScene.DrawCount(); ++draw)
    {
        const GLuint texture = gpuScene.TextureNameForDraw(draw);
        glUniform1i(hasTextureLoc, texture != 0 ? 1 : 0);
        if (texture != 0)
            glBindTextureUnit(0, texture);
        gpuScene.DrawOne(draw);
    }
}

