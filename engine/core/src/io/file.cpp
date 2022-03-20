#include <tempeh/io/file.hpp>
#include <cassert>

namespace Tempeh::IO
{
    File::File(std::FILE* fileptr) :
        m_fileptr(fileptr)
    {
    }

    std::optional<File> File::create(const std::string& filepath, FileMode mode)
    {
        std::FILE* fileptr;

        switch (mode) {
            case FileMode::Read:
                fileptr = std::fopen(filepath.c_str(), "rb");
                break;
            case FileMode::Write:
                fileptr = std::fopen(filepath.c_str(), "wb");
                break;
            case FileMode::ReadWrite:
                fileptr = std::fopen(filepath.c_str(), "r+b"); // try open the file
                
                if (fileptr == nullptr) {
                    fileptr = std::fopen(filepath.c_str(), "w+b");
                }

                break;
            default:
                return {};
        }

        if (fileptr == nullptr) {
            return {};
        }
        
        return { File(fileptr) };
    }

    std::optional<File> File::create_from_path(const std::filesystem::path& filepath, FileMode mode)
    {

        return std::optional<File>();
    }
}
