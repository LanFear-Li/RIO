#pragma once

#include <iostream>
#include <cstdint>

struct Texture
{
public:
    Texture(bool is_srgb, uint32_t width, uint32_t height, uint32_t nrComponents, const void *data);
    ~Texture();

    unsigned int get_id() const { return id; }

private:
    unsigned int id;
};
