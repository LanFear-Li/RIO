#pragma once

#include <glad/glad.h>

class Index_Buffer final
{
public:
    Index_Buffer() = delete;
    Index_Buffer(const void* data, unsigned int count);
    ~Index_Buffer();
    void bind() const;
    void unbind() const;

private:
    unsigned int ebo_id;
};
