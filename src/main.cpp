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


// Graphcis
#include "Importer/AssetImporter.h"
#include "Datas/DatasType.h"
#include "Engine/Gameplay/Input/KeyboardInput.h"
#include "Engine/Rendering/OpenGL/Texture.h"
#include "Engine/Rendering/OpenGL/Shader.h"
#include <miniaudio.h>
#include "Engine/Rendering/GPUScene.h"
#include "Engine/Rendering/RenderingManager.h"
//Audio

#include "Engine/Audio/Audio.h"


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
    "Assets/Shaders/vertex.shader",
    "Assets/Shaders/newvertex.shader"
};

std::vector<std::string> fragmentShaders{
    "Assets/Shaders/frag.shader",
    "Assets/Shaders/newfrag.shader"
};





void LoadAsset()
{
    std::cout << "Load Asset" << std::endl;
    AssetImporter importer;
    importer.LoadAsset("Assets/Models/hello.glb", meshes, transforms, drawColor);

    std::cout << "Showing temporary buffer" << std::endl;
    for (int i{}; i < meshes.size(); i++)
    {
        std::cout << "Mesh[" << i << "]" << std::endl;
        std::cout << "         VertexCount: " << meshes[i].vertexCount() << std::endl;
        std::cout << "         IndicesCount: " << meshes[i].indexCount() << std::endl;
    }
}


void initScene(GPUScene& gpuScene)
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

    }

//    uploadSSBOToGpu();

    gpuScene.Upload(
        allVertices,
        allIndices,
        transforms,
        drawMetadata,
        drawColor,
        drawCommands
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

    Audio music("Assets/Audio/drumsta.mp3");
    music.Play();

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



    if (!initOpenGL(windowWidth, windowHeight, "Drumsta LOVE"))
        return -1;


    Texture textureTest{ "Assets/Textures/palette.png" };
    //textures.push_back(textureTest);

    textureTest.UploadToGpu();

    GPUScene gpuScene;
    initScene(gpuScene);
    RenderingManager renderer;
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


        renderer.Render(
            viewProj,
            textureTest,
            gpuScene,
            shaderPrograms[0]
        );
        
     //   render(viewProj, textureTest);
       // render(viewProj, textureTest, gpuScene);

        glfwSwapBuffers(window);
    }
    glfwTerminate();
    return 0;
}
