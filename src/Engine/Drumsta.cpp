#include "Drumsta.h"

#include <glad/glad.h>
#include "Gameplay/Input/KeyboardInput.h"
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


Drumsta::Drumsta()
{

}

Drumsta::~Drumsta()
{

}


void Drumsta::Start()
{
	InitOpenGL();
    initInputDevice();
}


void Drumsta::initInputDevice()
{
    
    glfwSetKeyCallback(m_glContext.window, KeyboardInput::CurrentButtonPressedCallBack);
   
}

void Drumsta::loadTexture(
    const std::string& name,
    const std::string& path)
{
    auto [it, inserted] = m_Textures.try_emplace(name, path);
    it->second.UploadToGpu();
}

void Drumsta::InitOpenGL()
{
	m_glContext.setOpenGLContextDatas(1080, 720, "Engine", 4, 6);
	bool status = m_glContext.initOpenGL();

}


void Drumsta::processInput()
{


}

void Drumsta::Run()
{

    glm::vec3 cameraPos = glm::vec3(
        0.0f, 
        0.0f, 
        5.0f
    );


    while (!glfwWindowShouldClose(m_glContext.window))
    {

        glfwPollEvents();


        const float speed = 0.05f;
        if (KeyboardInput::IsKeyPressed(GLFW_KEY_W)) cameraPos.z -= speed;
        if (KeyboardInput::IsKeyPressed(GLFW_KEY_S)) cameraPos.z += speed;
        if (KeyboardInput::IsKeyPressed(GLFW_KEY_A)) cameraPos.x -= speed; 
        if (KeyboardInput::IsKeyPressed(GLFW_KEY_D)) cameraPos.x += speed;
        if (KeyboardInput::IsKeyPressed(GLFW_KEY_Q)) cameraPos.y += speed;
        if (KeyboardInput::IsKeyPressed(GLFW_KEY_E)) cameraPos.y -= speed; 

        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glm::mat4 view = glm::translate(glm::mat4(1.0f), -cameraPos);
        glm::mat4 proj = glm::perspective(glm::radians(45.0f),
            (float)m_glContext.getWindowsWidth() / (float)m_glContext.getWindowsheight(),
            0.1f, 100.0f);

        m_renderinManager.Render(proj * view, m_gpuScene, shadersPrograms["basicShader"]);
        glfwSwapBuffers(m_glContext.window);
    }
}

void Drumsta::ImportAsset(const std::string& filePathGLB)
{
	std::cout << "[ImportAsset] Loading " << filePathGLB << std::endl;
	m_importer.LoadAsset(filePathGLB, meshes, transforms, drawColor, materialDatas, imageDatas);
    


}



void Drumsta::InitScene()
{

    
    if (!GLAD_GL_ARB_bindless_texture)
    {
        std::cerr << "Update yourt GPU damn god !" << std::endl;
        return;
    }

 //   m_textureHandles.resize(imageDatas.size());


    //loadTexture("palette", "Assets/Textures/palette.png");
	Shader shader(vertexShaders[0], fragmentShaders[0]);
	shader.createShaderProgram();
	GLuint gpuID = shader.getGPUID();
	shadersPrograms["basicShader"] = gpuID;

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
        command.first = runningIndexOffset; 
        command.baseInstance = i;
        drawCommands.push_back(command);

        DrawMetadata meta;
        meta.baseVertex = runningVertexOffset;
        meta.materialIndex = mesh.materialId;
        meta.textureIndex = 0xFFFFFFFFu;
        if (mesh.materialId >= 0 && mesh.materialId < (int)materialDatas.size())
        {
            int tex = materialDatas[mesh.materialId].baseColorTextureIndex; 
            if (tex >= 0 && tex < (int)imageDatas.size())
                meta.textureIndex = uint32_t(tex);
        }
        drawMetadata.push_back(meta);


        runningVertexOffset += mesh.vertexCount();
        runningIndexOffset += mesh.indexCount();

    }
    m_gpuScene.Upload(allVertices,
        allIndices,
        transforms,
        drawMetadata,
        drawColor,
        drawCommands,
        //m_textureHandles
        imageDatas
    );
}
