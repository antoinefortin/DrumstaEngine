#include "Drumsta.h"

#include <glad/glad.h>
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
	//m_glContext.setOpenGLContextDatas(1080, 720, "Engine",4,6);
	InitOpenGL();
    
	//ImportAsset("");
}


void Drumsta::loadTexture(
    const std::string& name,
    const std::string& path)
{
    // Construct in place: copying a Texture would let the temporary's
    // destructor delete the GL texture the map still points to.
    auto [it, inserted] = m_Textures.try_emplace(name, path);
    it->second.UploadToGpu();
}

void Drumsta::InitOpenGL()
{
	m_glContext.setOpenGLContextDatas(1080, 720, "Engine", 4, 6);
	bool status = m_glContext.initOpenGL();

}

void Drumsta::Run()
{
    glm::vec3 cameraPos = glm::vec3(0.0f, 2.0f, 18.0f);
    while (!glfwWindowShouldClose(m_glContext.window))
    {
        glfwPollEvents();
        glClearColor(0.1f, 0.1f, 0.12f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
        
        glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f, 0.0f);

        glm::mat4 view = glm::lookAt(cameraPos, cameraFront, cameraUp);
        glm::mat4 proj = glm::perspective(glm::radians(45.0f),
            (float) m_glContext.getWindowsWidth()/ (float)m_glContext.getWindowsheight(),
            0.1f, 100.0f);

        glm::mat4 viewProj = proj * view;

        m_renderinManager.Render(viewProj,
            m_Textures.at("palette"),
            m_gpuScene,
            shadersPrograms["basicShader"]
            );
        glfwSwapBuffers(m_glContext.window);

    }
}

void Drumsta::ImportAsset(const std::string& filePathGLB)
{
	std::cout << "[ImportAsset] Loading " << filePathGLB << std::endl;
	m_importer.LoadAsset(filePathGLB, meshes, transforms, drawColor);
    
}



void Drumsta::InitScene()
{
    
    loadTexture("palette", "Assets/Textures/palette.png");
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
        drawMetadata.push_back(meta);
        runningVertexOffset += mesh.vertexCount();
        runningIndexOffset += mesh.indexCount();

    }
    m_gpuScene.Upload(allVertices,
        allIndices,
        transforms,
        drawMetadata,
        drawColor,
        drawCommands
    
    );
}
