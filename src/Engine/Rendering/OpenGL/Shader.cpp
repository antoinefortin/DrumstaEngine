#include "Shader.h"

Shader::Shader(
    const std::string& vertex,
    const std::string& frag
)
{
    vertexSourceCode = FileUtils::GetInstance().ReadTextFileLock(vertex);
    fragmentSourceCode = FileUtils::GetInstance().ReadTextFileLock(frag);


}

Shader::Shader(
    const std::string& compute
)
{

}
Shader::~Shader()
{

}



void Shader::createShaderProgram()
{
    if (vertexSourceCode.empty() || fragmentSourceCode.empty())
    {
        std::cerr << "Failed to load shader, kille.\n";
    }


    GLuint vertexShader = compileShaderStage(vertexSourceCode, GL_VERTEX_SHADER);
    GLuint fragmentShader = compileShaderStage(fragmentSourceCode, GL_FRAGMENT_SHADER);

    if (vertexShader == 0 || fragmentShader == 0)
    {
        if (vertexShader) glDeleteShader(vertexShader);
        if (fragmentShader) glDeleteShader(fragmentShader);
    }

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);


    GLint linkSuccess = 0;
    glGetProgramiv(program, GL_LINK_STATUS, &linkSuccess);
    if (program != 0)
    {
        shaderID = (program);
    }

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}
GLuint Shader::compileShaderStage(
    const std::string& source,
    GLenum shaderType
)
{
    GLuint shader = glCreateShader(shaderType);
    const char* src = source.c_str();

    glShaderSource(shader, 1, &src, nullptr);
    glCompileShader(shader);

    GLint success = 0;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);

    if (!success)
    {
        GLint logLength = 0;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

        std::vector<char> log(logLength > 0 ? logLength : 1);
        glGetShaderInfoLog(shader, logLength, nullptr, log.data());

        std::cerr << "Shader compile error (" << source << "):\n" << log.data() << "\n";

        glDeleteShader(shader);
        return 0;
    }
    return shader;
}


GLuint Shader::getGPUID()
{
    return shaderID;
}
