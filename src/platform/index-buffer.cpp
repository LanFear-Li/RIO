#include "index-buffer.hpp"

Index_Buffer::Index_Buffer(const void* data, unsigned int count)
{
    glGenBuffers(1, &ebo_id);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, count, data, GL_STATIC_DRAW);
}

Index_Buffer::~Index_Buffer()
{
    glDeleteBuffers(1, &ebo_id);
}

void Index_Buffer::bind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo_id);
}

void Index_Buffer::unbind() const
{
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
}
