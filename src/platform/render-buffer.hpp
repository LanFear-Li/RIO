#pragma once

#include <glad/glad.h>

class Render_Buffer final
{
public:
    Render_Buffer() = delete;
    Render_Buffer(int width, int height, GLenum buffer_format);
    Render_Buffer(GLuint buffer_id);
    ~Render_Buffer();

    void bind() const;
    void unbind() const;

    int get_width() const { return rbo_width; }
    int get_height() const { return rbo_height; }
    unsigned int get_id() const { return rbo_id; }

private:
    GLuint rbo_id;
    int rbo_width, rbo_height;
};
