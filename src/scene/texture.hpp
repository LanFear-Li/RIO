#pragma once

#include <iostream>
#include <cstdint>
#include <memory>
#include <vector>

enum Texture_Type
{
    TEXTURE_2D,
    TEXTURE_CUBE_MAP
};

struct Texture
{
public:
    Texture(unsigned int texture_id) : id(texture_id) {}
    ~Texture();
    unsigned int get_id() { return id; }
private:
    unsigned int id;
};

std::unique_ptr<Texture> load_texture_2d(uint32_t width, uint32_t height, uint32_t nrComponents, const void *data);

std::unique_ptr<Texture> load_cube_map(std::vector<std::string> faces, uint32_t nrComponents);
