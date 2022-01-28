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
        std::filesystem::path name;
        u32 size;
    };

    class FileTree
    {
    private:
        using FileTreeMap = std::map<std::filesystem::path, std::shared_ptr<FileTree>>;

        std::filesystem::path m_path;
        FileTreeMap m_file_map;

    public:
        FileTree();
        FileTree(const std::filesystem::path& path);

        // Implemented as BFS
        void traversal(std::function<void(FileMetaData)> f);
    };
}

#endif //_TEMPEH_FILE_FILETREE_HPP
