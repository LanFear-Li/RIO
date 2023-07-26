#pragma once

#include <string>
#include <cstdlib>

class FileSystem
{
private:
    typedef std::string (*Builder)(const std::string &path);

public:
    static std::string getPath(const std::string &path);

private:
    static std::string const &getRoot();

    //static std::string(*foo (std::string const &)) getPathBuilder()
    static Builder getPathBuilder()
    {
        if (!getRoot().empty())
            return &FileSystem::getPathRelativeRoot;
        else
            return &FileSystem::getPathRelativeBinary;
    }

    static std::string getPathRelativeRoot(const std::string &path);

    static std::string getPathRelativeBinary(const std::string &path);
};
