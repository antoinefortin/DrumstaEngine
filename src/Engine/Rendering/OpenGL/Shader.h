#pragma once
#include <iostream>
#include <vector>

#include "../../Utils/FileUtils.h"
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

class Shader
{
public:
    Shader(
        const std::string& vertex,
        const std::string& frag
        );

    Shader(
        const std::string& compute
    );
    ~Shader();

    GLuint createShaderProgram();
    GLuint compileShaderStage(
        const std::string& source,
        GLenum shaderType
    );
    //void changeVertexSource(const std::string in);
    //void changeFragmentSource(const std::string in);

private:
    std::string vertexSourceCode;
    std::string fragmentSourceCode;
    GLuint vtxID;
    GLuint fragID;

    GLuint shaderID;
};
