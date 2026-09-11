#pragma once
#include <stb_image.h>
#include <stb_image_write.h>
#include <string>
#include "../../../Datas/DatasType.h"
#include <stb_image_write.h>

class Texture
{
public:

    Texture();
    Texture(const std::string& path);
    CPU_Color GetPixelColor(const int& x,const int& y);
    void ShowTextureInfos();
    bool SaveToFile(const std::string& path) const;

private:
    unsigned char* textureData;
    int width, height, channels;

    std::string TexturePath;
};
