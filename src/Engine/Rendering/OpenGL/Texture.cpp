#include "Texture.h"
#include <iostream>



Texture::Texture()
{
    std::cout << "Empty texture";
    
}


Texture::Texture(const std::string& path)
{
    TexturePath = path;
    textureData =  stbi_load(path.c_str(), &width, &height, &channels, 4);
    if (textureData == NULL) {
        printf("Failed to load image: %s\n", stbi_failure_reason());
        return;
    }
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


bool Texture::SaveToFile(const std::string& path) const
{
    if (textureData == nullptr)
    {
        std::cerr << "Cannot save texture: textureData is null." << std::endl;
        return false;
    }

    int result = stbi_write_png(path.c_str(), width, height,4,textureData, width * 4);

    if (result == 0)
    {
        std::cerr << "Failed to save texture: " << path << std::endl;
        return false;
    }

    std::cout << "Texture saved: " << path << std::endl;
    return true;
}
