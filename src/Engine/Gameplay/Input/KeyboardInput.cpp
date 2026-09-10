#include "KeyboardInput.h"
#include <iostream>

std::unordered_map<int, bool> KeyboardInput::keyStates;

KeyboardInput::KeyboardInput()
{
    std::cout << "Keyboard system\n";
}

KeyboardInput::KeyboardInput(GLFWwindow* window)
{
    std::cout << "Keyboard system with callback\n";
    // everytime a key is rpesserd we set here the function to execute
    glfwSetKeyCallback(window, CurrentButtonPressedCallBack);
}

// This runs everytime a key is presed
void KeyboardInput::CurrentButtonPressedCallBack(GLFWwindow* window, int key, int scancode, int action, int mods)
{
    if (action == GLFW_PRESS)
    {
        keyStates[key] = true;
    }
        
    else if (action == GLFW_RELEASE)
    {
        keyStates[key] = false;
    }
}

bool KeyboardInput::IsKeyPressed(int key)
{
    auto it = keyStates.find(key);
    return it != keyStates.end() && it->second;
}
