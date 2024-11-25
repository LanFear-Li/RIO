#include "vertex-buffer.hpp"

Vertex_Buffer::Vertex_Buffer(const void* data, unsigned int size)
{
    glGenBuffers(1, &vbo_id);
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
    glBufferData(GL_ARRAY_BUFFER, size, data, GL_STATIC_DRAW);
}

Vertex_Buffer::~Vertex_Buffer()
{
    glDeleteBuffers(1, &vbo_id);
}

void Vertex_Buffer::bind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, vbo_id);
}

void Vertex_Buffer::unbind() const
{
    glBindBuffer(GL_ARRAY_BUFFER, 0);
}
