#pragma once

#include <string>
#include <mutex>

class FileUtils
{
public:
    static FileUtils& GetInstance()
    {
        static FileUtils instance;
        return instance;
    }

    FileUtils(const FileUtils&) = delete;
    FileUtils& operator=(const FileUtils&) = delete;
    FileUtils(FileUtils&&) = delete;
    FileUtils& operator=(FileUtils&&) = delete;

    // file sutff
    std::string ReadTextFileNoLock(const std::string& path) const;
    std::string ReadTextFileLock(const std::string& path);
    bool WriteTextFile(
        const std::string& path,
        const std::string& content
    );

    bool Exists(const std::string& path);

private:
    FileUtils() = default;
    ~FileUtils() = default;

    std::mutex m_mutex;
};
