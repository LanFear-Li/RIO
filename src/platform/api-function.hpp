#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

class Api_Function
{
public:
    static void set_viewport(int width, int height);

    static void clear_color(const glm::vec4 &color);
    static void clear_depth();
    static void clear();

    static void draw_arrays(GLenum mode, int first, int count);
    static void draw_elements(GLenum mode, int count, GLenum type, const void *indices);
};


