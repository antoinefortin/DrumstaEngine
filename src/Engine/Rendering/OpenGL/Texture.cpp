#include "Texture.h"
#include <iostream>



Texture::Texture()
{
    std::cout << "Empty texture";
    
}

Texture::Texture(const std::string& path)
    : textureData(nullptr),
    width(0),
    height(0),
    channels(0),
    gpuTextureID(0),
    textureFormat(GL_RGB)
{
    TexturePath = path;

    textureData = stbi_load(path.c_str(),&width,&height,&channels,0);

    if (!textureData)
    {
        std::cerr << "Failed to load image: "
            << stbi_failure_reason()
            << std::endl;
        return;
    }

    setTextureFormat();

    ShowTextureInfos();
}


void Texture::ShowTextureInfos()
{
    std::cout << "************************" << std::endl;
    std::cout << "*      Texture Infos   *" << std::endl;
    std::cout << "************************" << std::endl;
    std::cout << "\t Path: " << TexturePath << std::endl;
    std::cout << "\t Width: " << width << std::endl;
    std::cout << "\t Heifht: " << height << std::endl;

    
}


CPU_Color Texture::GetPixelColor(const int& _x, const int& _y)
{

    CPU_Color res{};
    int index = (_y * width + _x) * channels;
    // ecept is flat will alwayus bve rgb
    res.r = textureData[index];
    if (channels >= 2)
    {
        res.g = textureData[index + 1];
    }
    if (channels >= 3)
    {
        res.b = textureData[index + 2];
    }
    //if (channels >= 4) res.a = textureData[index + 3];


    return res;
}
GLuint Texture::getGPUHandle()
{
    return gpuTextureID;
}

bool Texture::SaveToFile(const std::string& path) const
{
    if (textureData == nullptr)
    {
        std::cerr << "Cannot save texture: textureData is null." << std::endl;
        return false;
    }

    int result = stbi_write_png(path.c_str(),width,height,channels,
        textureData,
        width * channels
    );


    if (result == 0)
    {
        std::cerr << "Failed to save texture: " << path << std::endl;
        return false;
    }

    std::cout << "Texture saved: " << path << std::endl;
    return true;
}

void Texture::UploadToGpu()
{
    if (!textureData) { return; }
    if (gpuTextureID == 0) { glGenTextures(1, &gpuTextureID); }

    glBindTexture(GL_TEXTURE_2D, gpuTextureID);
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    glTexImage2D(GL_TEXTURE_2D,0,textureFormat,width,height,0,textureFormat,GL_UNSIGNED_BYTE,textureData);
    glGenerateMipmap(GL_TEXTURE_2D);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_S,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_WRAP_T,GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void Texture::setTextureFormat()
{
    switch (channels)
    {
    case 1:
        textureFormat = GL_RED;
        break;

    case 2:
        textureFormat = GL_RG;
        break;

    case 3:
        textureFormat = GL_RGB;
        break;

    case 4:
        textureFormat = GL_RGBA;
        break;

    default:
        std::cerr << "Unsupported channel count: "
            << channels << std::endl;
        textureFormat = GL_RGBA;
        break;
    }
}


//https://wikis.khronos.org/opengl/Pixel_Transfer#Pixel_layout
void Texture::setTextureAlligmentForRGB()
{
    if (textureFormat == GL_RGB)
    {
        glPixelStorei(GL_UNPACK_ALIGNMENT, 1);
    }
}

bool Texture::existOnGpu()
{
    return glIsTexture(gpuTextureID) ? true : false;
}


Texture::Texture(Texture&& other) noexcept
    : textureData(other.textureData),
    width(other.width),
    height(other.height),
    channels(other.channels),
    gpuTextureID(other.gpuTextureID),
    TexturePath(std::move(other.TexturePath)),
    textureFormat(other.textureFormat)
{
    other.textureData = nullptr;
    other.gpuTextureID = 0;
}

Texture& Texture::operator=(Texture&& other) noexcept
{
    if (this != &other)
    {
        release();
        textureData = other.textureData;
        width = other.width;
        height = other.height;
        channels = other.channels;
        gpuTextureID = other.gpuTextureID;
        TexturePath = std::move(other.TexturePath);
        textureFormat = other.textureFormat;

        other.textureData = nullptr;
        other.gpuTextureID = 0;
    }
    return *this;
}

Texture::~Texture()
{
    release();
}

void Texture::release()
{
    if (textureData)
    {
        stbi_image_free(textureData);
        textureData = nullptr;
    }

    if (gpuTextureID != 0)
    {
        glDeleteTextures(1, &gpuTextureID);
        gpuTextureID = 0;
    }
}
