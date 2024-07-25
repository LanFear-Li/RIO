#include "filesystem.hpp"
#include <iostream>

std::string FileSystem::getPath(const std::string &path)
{
    static std::string (*pathBuilder)(std::string const &) = getPathBuilder();
    return (*pathBuilder)(path);
}

std::string const& FileSystem::getRoot()
{
    static char const *envRoot = getenv("LOGL_ROOT_PATH");
    static char const *givenRoot = (envRoot != nullptr ? envRoot : PROJECT_SOURCE_DIR);
    static std::string root = (givenRoot != nullptr ? givenRoot : "");
    return root;
}

std::string FileSystem::getPathRelativeRoot(const std::string &path)
{
    return getRoot() + std::string("/") + path;
}

std::string FileSystem::getPathRelativeBinary(const std::string &path)
{
    return "../../../" + path;
}
