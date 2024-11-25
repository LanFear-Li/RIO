#include "vertex-array.hpp"

Vertex_Array::Vertex_Array()
{
    glGenVertexArrays(1, &vao_id);
    glBindVertexArray(vao_id);
}

Vertex_Array::~Vertex_Array()
{
    glDeleteVertexArrays(1, &vao_id);
}

void Vertex_Array::bind() const
{
    glBindVertexArray(vao_id);
}

void Vertex_Array::unbind() const
{
    glBindVertexArray(0);
}

