#define NOMINMAX
#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <vector>
#include <random>
#include <chrono>
#include <cmath>
#include <map>
#include <algorithm>


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


// AFTER
#include "Importer/AssetImporter.h"
#include "Datas/DatasType.h"
#include "Engine/Gameplay/Input/KeyboardInput.h"
#include "Engine/Rendering/OpenGL/Texture.h"
#include "Engine/Rendering/OpenGL/Shader.h"

#include <stb_image.h>
#include <stb_image_write.h>

typedef GLuint gint;
GLFWwindow* window{ nullptr };

int windowWidth = 1280;
int windowHeight = 720;
void framebufferSizeCallback(GLFWwindow* window, int width, int height)
{
    windowWidth = width;
    windowHeight = height;
    glViewport(0, 0, width, height);
}
void glDebugCallback(GLenum source, GLenum type, GLuint id, GLenum severity, GLsizei length, const GLchar* message, const void* userParam)
{
    if (severity == GL_DEBUG_SEVERITY_NOTIFICATION)
        return;

    std::cerr << "[GL DEBUG] " << message << "\n";
}
bool initOpenGL(int width, int height, const char* title)
{
    if (!glfwInit())
    {
        std::cerr << "Failed to init GLFW\n";
        return false;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef _DEBUG
    glfwWindowHint(GLFW_OPENGL_DEBUG_CONTEXT, GLFW_TRUE);
#endif

    window = glfwCreateWindow(width, height, title, nullptr, nullptr);
    if (!window)
    {
        std::cerr << "Failed to create GLFW window\n";
        glfwTerminate();
        return false;
    }

    windowWidth = width;
    windowHeight = height;

    glfwMakeContextCurrent(window);
    glfwSetFramebufferSizeCallback(window, framebufferSizeCallback);
    glfwSwapInterval(1); // vsync

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
    {
        std::cerr << "Failed to init GLAD\n";
        return false;
    }

    std::cout << "OpenGL " << glGetString(GL_VERSION) << "\n";
    std::cout << "GPU: " << glGetString(GL_RENDERER) << "\n";

#ifdef _DEBUG
    glEnable(GL_DEBUG_OUTPUT);
    glEnable(GL_DEBUG_OUTPUT_SYNCHRONOUS);
    glDebugMessageCallback(glDebugCallback, nullptr);
#endif

    glEnable(GL_DEPTH_TEST);

    glViewport(0, 0, width, height);

    return true;
}



gint ssboVertex, ssboIndex, ssboTransform, ssboMetadata, indirectBuffer, vao, ssboColor;




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



std::vector<GLuint> shaderPrograms{};
std::vector<Mesh> meshes = {};
std::vector<Vertex>   allVertices;
std::vector<uint32_t> allIndices;
std::vector<DrawArraysIndirectCommand> drawCommands;
std::vector<DrawMetadata> drawMetadata;
std::vector<DrawColor> drawColor;
std::vector<glm::mat4x4> transforms;
std::vector<Texture> textures;



std::vector<std::string> vertexShaders{
    "Shaders/vertex.shader",
    "Shaders/newvertex.shader"
};

std::vector<std::string> fragmentShaders{
    "Shaders/frag.shader",
    "Shaders/newfrag.shader"
};



Mesh generateCube(float size = 1.0f)
{
    Mesh mesh;
    float h = size * 0.5f;

    mesh.verts = {
        { -h, -h, -h }, 
        {  h, -h, -h }, 
        {  h,  h, -h }, 
        { -h,  h, -h }, 
        { -h, -h,  h }, 
        {  h, -h,  h }, 
        {  h,  h,  h }, 
        { -h,  h,  h }, 
    };

    mesh.indices = {
        0, 1, 2,  2, 3, 0,
        4, 6, 5,  6, 4, 7,
        0, 3, 7,  7, 4, 0,
        1, 5, 6,  6, 2, 1,
        0, 4, 5,  5, 1, 0,
        3, 2, 6,  6, 7, 3,
    };

    return mesh;
}
Mesh generateIcosphere(float radius = 1.0f, int subdivisions = 1)
{
    Mesh mesh;
    const float t = (1.0f + std::sqrt(5.0f)) * 0.5f; 
    std::vector<Vertex> baseVerts = {
        {-1,  t,  0}, { 1,  t,  0}, {-1, -t,  0}, { 1, -t,  0},
        { 0, -1,  t}, { 0,  1,  t}, { 0, -1, -t}, { 0,  1, -t},
        { t,  0, -1}, { t,  0,  1}, {-t,  0, -1}, {-t,  0,  1},
    };
    auto normalize = [](Vertex v) -> Vertex {
        float len = std::sqrt(v.x * v.x + v.y * v.y + v.z * v.z);
        return { v.x / len, v.y / len, v.z / len };
        };

    for (auto& v : baseVerts)
        v = normalize(v);

    mesh.verts = baseVerts;

  
    std::vector<uint32_t> baseIndices = {
        0, 11, 5,   0, 5, 1,    0, 1, 7,    0, 7, 10,   0, 10, 11,
        1, 5, 9,    5, 11, 4,   11, 10, 2,  10, 7, 6,   7, 1, 8,
        3, 9, 4,    3, 4, 2,    3, 2, 6,    3, 6, 8,    3, 8, 9,
        4, 9, 5,    2, 4, 11,   6, 2, 10,   8, 6, 7,    9, 8, 1,
    };

    mesh.indices = baseIndices;
    std::map<uint64_t, uint32_t> midpointCache;

    auto getMidpoint = [&](uint32_t i0, uint32_t i1) -> uint32_t
        {
            uint64_t key = (std::min(i0, i1) < std::max(i0, i1))
                ? ((uint64_t)std::min(i0, i1) << 32) | std::max(i0, i1)
                : ((uint64_t)std::max(i0, i1) << 32) | std::min(i0, i1);

            auto it = midpointCache.find(key);
            if (it != midpointCache.end())
                return it->second;

            Vertex v0 = mesh.verts[i0];
            Vertex v1 = mesh.verts[i1];
            Vertex mid = {
                (v0.x + v1.x) * 0.5f,
                (v0.y + v1.y) * 0.5f,
                (v0.z + v1.z) * 0.5f
            };
            mid = normalize(mid);

            uint32_t newIndex = (uint32_t)mesh.verts.size();
            mesh.verts.push_back(mid);
            midpointCache[key] = newIndex;
            return newIndex;
        };

    for (int s = 0; s < subdivisions; ++s)
    {
        std::vector<uint32_t> newIndices;
        newIndices.reserve(mesh.indices.size() * 4);

        for (size_t i = 0; i < mesh.indices.size(); i += 3)
        {
            uint32_t i0 = mesh.indices[i];
            uint32_t i1 = mesh.indices[i + 1];
            uint32_t i2 = mesh.indices[i + 2];

            uint32_t a = getMidpoint(i0, i1);
            uint32_t b = getMidpoint(i1, i2);
            uint32_t c = getMidpoint(i2, i0);

            newIndices.insert(newIndices.end(), {
                i0, a, c,
                i1, b, a,
                i2, c, b,
                a,  b, c
                });
        }

        mesh.indices = newIndices;
        midpointCache.clear(); 
    }

    for (auto& v : mesh.verts)
    {
        v.x *= radius;
        v.y *= radius;
        v.z *= radius;
    }

    return mesh;
}
void createMeshData()
{
  //  meshes.push_back(generateCube(1.0f));
  // meshes.push_back(generateIcosphere(1.0f, 2));
}

void uploadSSBOToGpu()
{
    glGenVertexArrays(1, &vao);
    // Vertex SBO
    glGenBuffers(1, &ssboVertex);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboVertex);
    glBufferData(GL_SHADER_STORAGE_BUFFER, allVertices.size() * sizeof(Vertex), allVertices.data(), GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, ssboVertex);

    glGenBuffers(1, &ssboIndex);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboIndex);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        allIndices.size() * sizeof(uint32_t),
        allIndices.data(),
        GL_STATIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, ssboIndex);

    glGenBuffers(1, &ssboTransform);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboTransform);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        transforms.size() * sizeof(glm::mat4), // ou ton type de matrice
        transforms.data(),
        GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 2, ssboTransform);


    glGenBuffers(1, &ssboMetadata);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboMetadata);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        drawMetadata.size() * sizeof(DrawMetadata),
        drawMetadata.data(),
        GL_DYNAMIC_DRAW);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 3, ssboMetadata);
    
    glGenBuffers(1, &ssboColor);
    glBindBuffer(GL_SHADER_STORAGE_BUFFER, ssboColor);
    glBufferData(GL_SHADER_STORAGE_BUFFER,
        drawColor.size() * sizeof(DrawColor),
        drawColor.data(),
        GL_DYNAMIC_DRAW);

    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 4, ssboColor);

    // GPU draw command
    glGenBuffers(1, &indirectBuffer);
    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
    glBufferData(GL_DRAW_INDIRECT_BUFFER,
        drawCommands.size() * sizeof(DrawArraysIndirectCommand),
        drawCommands.data(),
        GL_DYNAMIC_DRAW);

}

void LoadAsset()
{
    std::cout << "Load Asset" << std::endl;
    AssetImporter importer;
    importer.LoadAsset("C:/dev/assets/Untitled.glb", meshes, transforms, drawColor);

    std::cout << "Showing temporary buffer" << std::endl;
    for (int i{}; i < meshes.size(); i++)
    {
        std::cout << "Mesh[" << i << "]" << std::endl;
        std::cout << "         VertexCount: " << meshes[i].vertexCount() << std::endl;
        std::cout << "         IndicesCount: " << meshes[i].indexCount() << std::endl;
    }
}


void initScene()
{

    LoadAsset();


    // Shader Test
    Shader shader(vertexShaders[0], fragmentShaders[0]);
    shader.createShaderProgram();
    GLuint gpuShaderDI = shader.getGPUID();
    shaderPrograms.push_back(gpuShaderDI);
    /*
        That Shit needs refactor so SSBO class 
        THIS IS NEEDED AS FUCKING SOON AS POSSIBLE
        HARDCODED SHIT
    */
 //   createMeshData();

    uint32_t runningVertexOffset{ 0 };
    uint32_t runningIndexOffset{ 0 };
    std::cout << "Mesh Size: " << meshes.size() << std::endl;
    std::cout << "Transform Size: " << transforms.size() << std::endl;
    // Vertex
    for (int i{}; i < meshes.size(); i++)
    {
        const Mesh& mesh = meshes[i];
        allVertices.insert(allVertices.end(), mesh.verts.begin(), mesh.verts.end());
        allIndices.insert(allIndices.end(), mesh.indices.begin(), mesh.indices.end());

        DrawArraysIndirectCommand command;
        command.count = mesh.indexCount();
        command.instanceCount = 1;
        command.first = runningIndexOffset; // corrigé
        command.baseInstance = i;
        drawCommands.push_back(command);

        DrawMetadata meta;
        meta.baseVertex = runningVertexOffset;
        meta.materialIndex = mesh.materialId;
        drawMetadata.push_back(meta);



        runningVertexOffset += mesh.vertexCount();
        runningIndexOffset += mesh.indexCount();


        //transforms.push_back(glm::translate(glm::mat4(1.0f), glm::vec3(-2.0f, 0.0f, 0.0f)));

    }

    uploadSSBOToGpu();
}
void render(const glm::mat4& viewProj)
{

    std::cout << 1;
    glUseProgram(shaderPrograms[0]); 

    GLint locViewProj = glGetUniformLocation(shaderPrograms[0], "viewProj");
    glUniformMatrix4fv(locViewProj, 1, GL_FALSE, glm::value_ptr(viewProj));

    GLint locLightDir = glGetUniformLocation(shaderPrograms[0], "lightDir");
    glUniform3f(locLightDir, 0.4f, 0.8f, 0.3f);

    GLint locLightColor = glGetUniformLocation(shaderPrograms[0], "lightColor");
    glUniform3f(locLightColor, 1.0f, 1.0f, 1.0f);

    GLint locBaseColor = glGetUniformLocation(shaderPrograms[0], "baseColor");
    glUniform3f(locBaseColor, 0.8f, 0.8f, 0.8f);

    glBindVertexArray(vao);

    glBindBuffer(GL_DRAW_INDIRECT_BUFFER, indirectBuffer);
    glMultiDrawArraysIndirect(
        GL_TRIANGLES,
        (void*)0,
        (GLsizei)drawCommands.size(),
        0
    );
}



void HandleCameraMovement(
    const KeyboardInput& inputDevice,
    glm::vec3& cameraPos,
    const float& camMovementSpeed

)
{

        //processInput(window, cameraPos);
        // -Z is away froim screren
        if (KeyboardInput::IsKeyPressed(GLFW_KEY_W))
        {
            cameraPos.z -= camMovementSpeed;
        }
        // +Z is into the screen
        if (KeyboardInput::IsKeyPressed(GLFW_KEY_S))
        {
            cameraPos.z += camMovementSpeed;
        }

        // -x is left
        if (KeyboardInput::IsKeyPressed(GLFW_KEY_A))
        {
            cameraPos.x -= camMovementSpeed;
        }
        // +x is right
        if (KeyboardInput::IsKeyPressed(GLFW_KEY_D))
        {
            cameraPos.x += camMovementSpeed;
        }

        // Up 
        if (KeyboardInput::IsKeyPressed(GLFW_KEY_Q))
        {
            cameraPos.y += camMovementSpeed;
        }
        // Down
        if (KeyboardInput::IsKeyPressed(GLFW_KEY_E))
        {
            cameraPos.y -= camMovementSpeed;
        }
}

int main()
{



   /*

   
    GLuint handle = textureTest.getGPUHandle();
    bool isUplaoded = textureTest.existOnGpu();
    if (isUplaoded)
    {
        std::cout << "Isd on GPU : " << "GPU Texture ID: " << handle << std::endl;
    }
    else
    {
        std::cout << "Not uploaded" << std::endl;
    }
    // test
    std::vector<CPU_Color> testSampling{};
    testSampling.reserve(3);
    testSampling.push_back(textureTest.GetPixelColor(1, 0)); // (255, 0 , 0 )
    testSampling.push_back(textureTest.GetPixelColor(45, 0));// (0, 255 , 0 )
    testSampling.push_back(textureTest.GetPixelColor(1, 35));// (0, 0, 255)

    for (int i{}; i < testSampling.size(); i++)
    {
        CPU_Color curr = testSampling[i];
        std::cout << "R: " << curr.r << " G: " << curr.g << "B: " << curr.b << std::endl;
    }
    
    textureTest.SaveToFile("Assets/Textures/poney.png");
    */
    //textureTest.UploadToGpu();


    // test texture upl;oad to GPU
    // 
//    std::cout << textureTest.GetPixelColor(0, 0).r << ;
    //return 0;



    if (!initOpenGL(windowWidth, windowHeight, "OpenGL Sandbox"))
        return -1;


    Texture textureTest{ "Assets/Textures/debug.png" };
    textures.push_back(textureTest);

    textureTest.UploadToGpu();
    if (textureTest.existOnGpu())
    {
        std::cout << "Texture data existe on GPU Vram :" << std::endl;
        std::cout << "       GPUHandleID  -> " << (int)textureTest.getGPUHandle() << std::endl;
    }

    std::cout << "breakpoiint";
        
    initScene();
    glm::vec3 cameraPos = glm::vec3(0.0f, 2.0f, 18.0f);
    KeyboardInput input(window); // 


    glEnable(GL_DEPTH_TEST);


    while (!glfwWindowShouldClose(window))
    {
        glfwPollEvents();
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        HandleCameraMovement(input, cameraPos, 0.25f);

        glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f); 
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::mat4 view = glm::lookAt(cameraPos, cameraFront, cameraUp);
        glm::mat4 proj = glm::perspective(glm::radians(45.0f),
            (float)windowWidth / (float)windowHeight,
            0.1f, 100.0f);
        glm::mat4 viewProj = proj * view;

        render(viewProj);

        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}
