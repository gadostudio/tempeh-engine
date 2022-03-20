#include <tempeh/io/basic.hpp>
#include <fstream>

namespace Tempeh::IO
{
    std::optional<std::vector<char>> read_binary_data_from_file(const std::string& filepath,
                                                                std::ptrdiff_t offset,
                                                                std::size_t size)
    {
        std::vector<char> ret;
        std::ifstream file(filepath);

        if (!file.is_open()) {
            return {};
        }

        return ret;
    }

    std::optional<std::vector<char>> read_binary_data_from_file(const std::filesystem::path& filepath,
                                                                std::ptrdiff_t offset,
                                                                std::size_t size)
    {
        return read_binary_data_from_file(filepath.string());
    }
}
