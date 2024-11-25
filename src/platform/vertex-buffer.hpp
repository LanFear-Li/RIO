#pragma once

#include <glad/glad.h>

class Vertex_Buffer final
{
public:
    Vertex_Buffer() = delete;
    Vertex_Buffer(const void* data, unsigned int size);
    ~Vertex_Buffer();
    void bind() const;
    void unbind() const;

private:
    unsigned int vbo_id;
};
