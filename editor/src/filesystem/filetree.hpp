#ifndef _TEMPEH_FILE_FILETREE_HPP
#define _TEMPEH_FILE_FILETREE_HPP

#include <memory>
#include <map>
#include <deque>
#include <filesystem>
#include <functional>
#include <tempeh/common/typedefs.hpp>

namespace TempehEditor::FileSystem
{

    struct FileMetaData
    {
        std::string name;
        u32 size;
    };

    class FileTree
    {
    private:
        using FileTreeMap = std::map<std::string, std::shared_ptr<FileTree>>;
        FileTreeMap hashmap;
    public:
        FileTree(const std::filesystem::path &path_str);



        // Implemented as BFS
        void traversal(std::function<void(FileMetaData)> f);
    };
}

#endif //_TEMPEH_FILE_FILETREE_HPP
