#ifndef _TEMPEH_IO_FILE_HPP
#define _TEMPEH_IO_FILE_HPP 1

#include <tempeh/common/util.hpp>
#include <filesystem>
#include <string_view>
#include <cstdio>
#include <optional>

namespace Tempeh::IO
{
    enum class FileMode
    {
        Read,
        Write,
        ReadWrite
    };

    class File
    {
    public:
        std::size_t read(void* out_data, std::size_t size);
        std::size_t write(const void* in_data, std::size_t size);
        std::size_t size() const;

    private:
        std::FILE* m_fileptr;

        File(std::FILE* fileptr);

        static std::optional<File> create(const std::string& filepath, FileMode mode);
        static std::optional<File> create(const char* filepath, std::size_t count, FileMode mode);
        static std::optional<File> create_from_path(const std::filesystem::path& filepath, FileMode mode);
    };
}

#endif