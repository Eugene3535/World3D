#include "files/FileProvider.hpp"

std::filesystem::path FileProvider::findPathToFile(const std::string& filename) noexcept
{
    static std::filesystem::path resource_folder;

    if (resource_folder.empty())
        resource_folder = std::filesystem::current_path() / "res";

    for (const auto& file : std::filesystem::recursive_directory_iterator(resource_folder))
        if (file.path().filename() == filename)    
            return file.path();
        
    return std::filesystem::path();
}