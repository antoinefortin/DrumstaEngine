cmake_minimum_required(VERSION 3.16)
project(OpenGLMac LANGUAGES CXX)

find_package(OpenGL REQUIRED)
find_package(glfw3 3.3 CONFIG REQUIRED)

add_executable(opengl_mac main.cpp)
target_compile_features(opengl_mac PRIVATE cxx_std_17)
target_compile_definitions(opengl_mac PRIVATE GL_SILENCE_DEPRECATION)
target_link_libraries(opengl_mac PRIVATE glfw OpenGL::GL)
