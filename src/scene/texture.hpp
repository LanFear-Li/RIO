#pragma once

#include <iostream>

// struct Texture
// {
// public:
//     Texture();
//     ~Texture();
//
//     unsigned int get_id() const { return texture_id; }
//
// private:
//     unsigned int texture_id;
// };

struct Texture
{
    unsigned int id;
    std::string type;
    std::string path;
};
