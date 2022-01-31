#ifndef TEMPEH_ENGINE_EDITOR_SRC_CORE_PROJECTS_HPP
#define TEMPEH_ENGINE_EDITOR_SRC_CORE_PROJECTS_HPP

#include <filesystem>
#include <tempeh/common/sbstd.hpp>

namespace TempehEditor::Projects
{

    struct ConfigAndroid
    {
        std::string package_name;
    };

    struct ConfigApplication
    {
        std::string name;
        sbstd::Optional<std::string> main_scene;
    };

    struct Config
    {
        ConfigApplication application;
        ConfigAndroid android;
    };

    class Project
    {
    private:
        std::filesystem::path m_project_path;
        Config m_config;
    public:
        Project(const std::filesystem::path& m_project_path);
        const Config& get_config() { return m_config; };
        void save();
    };

}

#endif //TEMPEH_ENGINE_EDITOR_SRC_CORE_PROJECTS_HPP
