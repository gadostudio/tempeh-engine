#include "filetree.hpp"

#include <memory>
#include <map>
#include <deque>
#include <filesystem>
#include <functional>
#include <tempeh/common/typedefs.hpp>
#include <cassert>
#include <system_error>
#include <list>

namespace TempehEditor::FileSystem {

    namespace fs = std::filesystem;

    FileTree::FileTree(const fs::path &path_str)
    {
        std::list<FileTreeMap::iterator> list;

        auto [iter, _] = hashmap.insert_or_assign(path_str.string(),
                                                  nullptr);

        list.push_back(iter);

        while (!list.empty()) {
            auto current_entry = list.front();
            list.pop_front();
            for (const auto & entry : fs::directory_iterator(current_entry->first)) {

                fs::path p = entry.path();

                std::shared_ptr<FileTree> tree;

                if (fs::path(p).filename() == ".git") {
                    continue;
                }
                if (fs::is_directory(p)) {
                    tree = std::make_shared<FileTree>(p);
                }

                auto [i, _] = hashmap.insert_or_assign(p.string(), tree);
                list.push_back(i);
            }
        }
    }

    // TODO
    void FileTree::traversal(std::function<void(FileMetaData)> f)
    {

    }

}