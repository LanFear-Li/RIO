#include "filesystem.hpp"

#include <iostream>

std::string File_System::get_path(const std::string &path)
{
    static std::string (*pathBuilder)(std::string const &) = get_path_builder();
    return (*pathBuilder)(path);
}

std::string const& File_System::get_root()
{
    static char const *envRoot = getenv("LOGL_ROOT_PATH");
    static char const *givenRoot = (envRoot != nullptr ? envRoot : PROJECT_SOURCE_DIR);
    static std::string root = (givenRoot != nullptr ? givenRoot : "");
    return root;
}

std::string File_System::get_path_relative_root(const std::string &path)
{
    return get_root() + std::string("/") + path;
}

std::string File_System::get_path_relative_binary(const std::string &path)
{
    return "../../../" + path;
}
