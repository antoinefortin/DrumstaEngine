#pragma once
#include <stb_image.h>
#include <string>
#include "../../../Datas/DatasType.h"

class Texture
{
public:

    Texture();
    Texture(const std::string& path);
    CPU_Color GetPixelColor(const int& x,const int& y);
    void ShowTextureInfos();

private:
    unsigned char* textureData;
    int width, height, channels;

    std::string TexturePath;
};
