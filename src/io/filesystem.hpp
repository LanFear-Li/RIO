#pragma once

#include <string>
#include <cstdlib>

class File_System final
{
private:
    typedef std::string (*builder)(const std::string &path);

public:
    static std::string get_path(const std::string &path);

private:
    static std::string const &get_root();

    static builder get_path_builder()
    {
        if (!get_root().empty()) {
            return &File_System::get_path_relative_root;
        } else {
            return &File_System::get_path_relative_binary;
        }
    }

    static std::string get_path_relative_root(const std::string &path);

    static std::string get_path_relative_binary(const std::string &path);
};
