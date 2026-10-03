#pragma once

#include <unordered_map>

#include "Rendering/GPUScene.h"
#include "../Importer/AssetImporter.h"
#include "Rendering/OpenGL/OpenGLContext.h"
#include "Rendering/RenderingManager.h"
#include "Rendering/OpenGL/Texture.h"
#include "Rendering/OpenGL/Shader.h"
#include "Gameplay/Input/KeyboardInput.h"


#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Drumsta
{

public:
	Drumsta();
	~Drumsta();
	
	KeyboardInput keyboardDevice{};
	std::vector<std::string> vertexShaders{
	"Assets/Shaders/vertex.shader",
	"Assets/Shaders/newvertex.shader"
	};

	std::vector<std::string> fragmentShaders{
		"Assets/Shaders/frag.shader",
		"Assets/Shaders/newfrag.shader"
	};

	void Start();




	void Run();
	
	void ImportAsset(const std::string& filePathGLB);
	void InitScene();

private:
	AssetImporter m_importer;
	OpenGLContext m_glContext;
	RenderingManager m_renderinManager = RenderingManager{};


	void loadTexture(
		const std::string& name,
		const std::string& path
	);
	void InitOpenGL();


	void initInputDevice();


	void createSSBOData();
	void processInput();


	//std::vector<GLuint> shaderPrograms{};
	std::vector<Mesh> meshes = {};
	std::vector<Vertex>   allVertices;
	std::vector<uint32_t> allIndices;
	std::vector<DrawArraysIndirectCommand> drawCommands;
	std::vector<DrawMetadata> drawMetadata;
	std::vector<DrawColor> drawColor;
	std::vector<glm::mat4x4> transforms;
	std::vector<Texture> textures;
    std::vector<MaterialData> materialDatas;
    std::vector<ImageData> imageDatas;

    std::unordered_map<std::string, GLuint> shadersPrograms;
	std::unordered_map<std::string, Texture> m_Textures;

    std::vector<GLuint> m_glTextures; // Texutre handles
    std::vector<GLuint64> m_textureHandles;

    GPUScene m_gpuScene = GPUScene{};
    GLuint m_textureHandleSSBO = 0;
    GLuint m_materialSSBO = 0;


};
