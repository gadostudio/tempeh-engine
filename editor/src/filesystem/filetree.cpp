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
    
    FileTree::FileTree()
    {
    }

    FileTree::FileTree(const std::filesystem::path& path) :
        m_path(path)
    {
        std::deque<std::pair<std::filesystem::path, FileTreeMap&>> queue;

        queue.emplace_back(m_path, m_path);

        while (!queue.empty()) {
            auto [current, current_map] = queue.back();
            queue.pop_back();

            for (const auto& entry : std::filesystem::directory_iterator{ current }) {
                std::filesystem::path p = entry.path();
                std::shared_ptr<FileTree> dir;

                if (std::filesystem::is_directory(p)) {
                    dir = std::make_shared<FileTree>();
                    queue.emplace_back(p, dir->m_path);
                }

                current_map.insert_or_assign(p, dir);
            }
        }
    }

    // TODO
    void FileTree::traversal(std::function<void(FileMetaData)> f)
    {

    }

}