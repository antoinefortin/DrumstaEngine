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

    CPU_Color GetPixelColor(const int& x,const int& y);
    void ShowTextureInfos();
    bool SaveToFile(const std::string& path) const;
    GLuint getGPUHandle();
    void UploadToGpu();
    bool existOnGpu();
private:
    unsigned char* textureData;
    int width, height, channels;
    GLuint gpuTextureID;
    std::string TexturePath;
    GLenum textureFormat;
    void setTextureFormat();
    void setTextureAlligmentForRGB();
};
