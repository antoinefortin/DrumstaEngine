#include "FileUtils.h"

#include <fstream>
#include <sstream>
#include <filesystem>
#include <stdexcept>


// old junk before meyer singleton need to delete
std::string FileUtils::ReadTextFileNoLock(const std::string& path) const 
{
    std::ifstream file(path);

    if (!file.is_open())
    {
        throw std::runtime_error(
            "Failed to open file: " + path
        );
    }

    std::stringstream buffer;
    buffer << file.rdbuf();

    return buffer.str();

}


std::string FileUtils::ReadTextFileLock(const std::string& path) 
{
    m_mutex.lock();
    std::ifstream file(path);
    if (!file.is_open())
    {throw std::runtime_error(
            "Failed to open file: " + path
        );
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    m_mutex.unlock();
    return buffer.str();

}

bool FileUtils::WriteTextFile(
    const std::string& path,
    const std::string& content)
{
    std::lock_guard<std::mutex> lock(m_mutex);
    std::ofstream file(path);
    if (!file.is_open())
    {
        return false;
    }
        

    file << content;
    return file.good();
}

bool FileUtils::Exists(const std::string& path)
{
    std::lock_guard<std::mutex> lock(m_mutex);

    return std::filesystem::exists(path);
}


