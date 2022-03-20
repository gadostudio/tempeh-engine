#ifndef _TEMPEH_IO_BASIC_HPP
#define _TEMPEH_IO_BASIC_HPP 1

#include <tempeh/common/typedefs.hpp>
#include <filesystem>
#include <string>
#include <vector>
#include <optional>

namespace Tempeh::IO
{
    std::optional<std::vector<char>> read_binary_data_from_file(const std::string& filepath,
                                                                std::ptrdiff_t offset = 0,
                                                                std::size_t size = ~static_cast<std::size_t>(0));

    std::optional<std::vector<char>> read_binary_data_from_file(const std::filesystem::path& filepath,
                                                                std::ptrdiff_t offset = 0,
                                                                std::size_t size = ~static_cast<std::size_t>(0));

    std::optional<std::string> read_string_from_file(const std::string& filepath);
    std::optional<std::string> read_string_from_file(const std::filesystem::path& filepath);
}

#endif