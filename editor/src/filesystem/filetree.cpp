#include "filetree.hpp"

#include <memory>
#include <map>
#include <deque>
#include <filesystem>
#include <functional>
#include <tempeh/common/typedefs.hpp>
#include <cassert>
#include <system_error>

namespace Tempeh::File {

    FileTree::FileTree(std::string x) {
        std::error_code err;
        std::filesystem::path y(x);
        assert(std::filesystem::exists(y, err));
    };
}