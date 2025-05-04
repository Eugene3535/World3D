#ifndef FILE_PROVIDER_HPP
#define FILE_PROVIDER_HPP

#include <string>
#include <filesystem>

#include "Export.hpp"

struct SHINY_API FileProvider
{
    static std::filesystem::path findPathToFile(const std::string& filename) noexcept;
};

#endif // !FILE_PROVIDER_HPP