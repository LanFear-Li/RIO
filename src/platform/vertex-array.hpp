#pragma once

#include <glad/glad.h>

class Vertex_Array final
{
public:
    Vertex_Array();
    ~Vertex_Array();

    void bind() const;
    void unbind() const;

private:
    GLuint vao_id;
};
