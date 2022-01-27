//
// Created by rahman on 1/27/22.
//

#ifndef TEMPEH_ENGINE_FILETREE_HPP
#define TEMPEH_ENGINE_FILETREE_HPP

#include <memory>
#include <map>
#include <deque>
#include <filesystem>
#include <functional>
#include <tempeh/common/typedefs.hpp>

namespace Tempeh::File {

    struct FileMetaData {
        std::string name;
        u32 size;
    };

    class FileTree {
    private:
        class FileTreeInternal {
            std::map<std::string, std::shared_ptr<FileTree>> hashmap;
        };
        std::unique_ptr<FileTreeInternal> root;
    public:
        FileTree(std::string path);

        // TODO
        void tree_traversal(std::function<void(FileMetaData)>);
    };
}

#endif //TEMPEH_ENGINE_FILETREE_HPP
