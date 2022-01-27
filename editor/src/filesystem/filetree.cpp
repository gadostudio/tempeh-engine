#include "filetree.hpp"

#include <memory>
#include <map>
#include <deque>
#include <filesystem>
#include <functional>
#include <tempeh/common/typedefs.hpp>
#include <cassert>
#include <system_error>

namespace TempehEditor::FileSystem {

    FileTree::FileTree(std::string& path_str)
    {
        std::filesystem::path path(path_str);
        std::error_code err;
        assert(std::filesystem::exists(path, err) && "Path did not exists");
    }

    // TODO
    void FileTree::traversal(std::function<void(FileMetaData)> f)
    {

    }

}