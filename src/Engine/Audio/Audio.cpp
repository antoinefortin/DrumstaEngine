#include "Audio.h"

#include <iostream>

Audio::Audio(const std::string& filepath)
{
    if (ma_engine_init(nullptr, &m_engine) != MA_SUCCESS)
    {
        std::cerr << "Failed to initialize audio engine\n";
        return;
    }

    m_engineInitialized = true;

    if (ma_sound_init_from_file(&m_engine,
        filepath.c_str(),0,
        nullptr,
        nullptr,
        &m_sound
    ) != MA_SUCCESS)
    {
        std::cerr << "Failed to load audio file: "
            << filepath
            << '\n';

        return;
    }

    m_soundInitialized = true;
}

Audio::~Audio()
{
    if (m_soundInitialized)
    {
        ma_sound_uninit(&m_sound);
    }

    if (m_engineInitialized)
    {
        ma_engine_uninit(&m_engine);
    }
}

void Audio::Play()
{
    if (m_soundInitialized)
    {
        ma_sound_start(&m_sound);
    }
}
