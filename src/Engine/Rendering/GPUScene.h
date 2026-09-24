#pragma once
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

/*
    CPU acxcessible data on top of GPU already uploaded data.
*/

class GPUScene
{

public:

    GPUScene() = default;
    ~GPUScene();
    GPUScene(const GPUScene&) = delete;
    GPUScene& operator=(const GPUScene&) = delete;

    void Bind() const;
    void Draw() const;

private:
    GLuint ssboVertex = 0;
    GLuint ssboIndex = 0;
    GLuint ssboTransform = 0;
    GLuint ssboMetadata = 0;
    GLuint ssboColor = 0;

    GLuint indirectBuffer = 0;
    GLuint vao = 0;

    GLsizei drawCount = 0;
};




