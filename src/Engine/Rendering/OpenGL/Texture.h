#pragma once
#include <stb_image.h>
#include <stb_image_write.h>
#include <string>
#include "../../../Datas/DatasType.h"
#include <stb_image_write.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>


class Texture
{
public:

    Texture();
    Texture(const std::string& path);
    ~Texture();

    // Owns a GL texture + stb pixels: no copies, moves only.
    Texture(const Texture&) = delete;
    Texture& operator=(const Texture&) = delete;
    Texture(Texture&& other) noexcept;
    Texture& operator=(Texture&& other) noexcept;

    CPU_Color GetPixelColor(const int& x,const int& y);
    void ShowTextureInfos();
    bool SaveToFile(const std::string& path) const;
    GLuint getGPUHandle();
    void UploadToGpu();
    bool existOnGpu();
private:
    unsigned char* textureData{ nullptr };
    int width{ 0 }, height{ 0 }, channels{ 0 };
    GLuint gpuTextureID{ 0 };
    std::string TexturePath;
    GLenum textureFormat{ GL_RGB };
    void release();
    void setTextureFormat();
    void setTextureAlligmentForRGB();
};
