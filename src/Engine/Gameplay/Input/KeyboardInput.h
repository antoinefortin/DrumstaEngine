#pragma once
#include <GLFW/glfw3.h>
#include "../../../Datas/DatasType.h"
#include <unordered_map>
class KeyboardInput
{


    public:

        KeyboardInput();
        KeyboardInput(GLFWwindow* window);
        static void CurrentButtonPressedCallBack(GLFWwindow* window, int key, int scancode, int action, int mods);
        static bool IsKeyPressed(int key);

        static void debugShit();


    private:
        static std::unordered_map<int, bool> keyStates;
};
