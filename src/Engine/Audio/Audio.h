#pragma once

#include <string>
#include <miniaudio.h>

class Audio
{
public:
    explicit Audio(const std::string& filepath);
    ~Audio();

    void Play();

private:
    ma_engine m_engine{};
    ma_sound m_sound{};

    bool m_engineInitialized = false;
    bool m_soundInitialized = false;
};
