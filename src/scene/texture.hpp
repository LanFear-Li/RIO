#pragma once

#include <iostream>
#include <memory>
#include <vector>

enum Texture_Type
{
    TEXTURE_2D,
    TEXTURE_EQUIRECTANGULAR,
    TEXTURE_CUBE_MAP,
    TEXTURE_2D_DEPTH
};

class Texture final
{
public:
    Texture(unsigned int texture_id, Texture_Type texture_type) : id(texture_id), type(texture_type) {}
    ~Texture();

    unsigned int get_id() const { return id; }
    Texture_Type get_type() const { return type; }

private:
    unsigned int id;
    Texture_Type type{Texture_Type::TEXTURE_2D};
};

std::unique_ptr<Texture> load_texture_2d(uint32_t width, uint32_t height, uint32_t n_components, const void *data);
std::shared_ptr<Texture> load_rect_map(std::string file_path, uint32_t n_components);
std::shared_ptr<Texture> load_cube_map(std::vector<std::string> faces, uint32_t n_components);
std::unique_ptr<Texture> create_texture(Texture_Type texture_type, int width, int height, bool mipmap=false);
