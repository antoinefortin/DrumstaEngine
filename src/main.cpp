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

#include "Engine/Drumsta.h"



// Graphcis
#include "Importer/AssetImporter.h"
#include "Datas/DatasType.h"
#include "Engine/Gameplay/Input/KeyboardInput.h"
#include "Engine/Rendering/OpenGL/Texture.h"
#include "Engine/Rendering/OpenGL/Shader.h"
#include <miniaudio.h>
#include "Engine/Rendering/GPUScene.h"
#include "Engine/Rendering/RenderingManager.h"
#include "Engine/Geometries/MeshGenerator.h"

//Audio

#include "Engine/Audio/Audio.h"


#include <stb_image.h>
#include <stb_image_write.h>







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
    void();
    Drumsta app{};
    app.Start();
    app.ImportAsset("Assets/Models/hello.glb");
    app.InitScene();

    app.Run();
    return 1;

}
