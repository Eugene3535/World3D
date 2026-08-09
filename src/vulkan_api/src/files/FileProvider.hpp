#pragma once

#include <string>
#include <filesystem>



class FileProvider
{
public:
    FileProvider() noexcept;

    static std::filesystem::path findPathToFile(const std::string& filename) noexcept;

private:
    std::filesystem::path m_exeDir;
};
