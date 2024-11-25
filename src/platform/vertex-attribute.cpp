#include "vertex-attribute.hpp"

#include <iostream>

Vertex_Attribute::Vertex_Attribute(GLsizei stride)
    : index(0), stride(stride) {}


void Vertex_Attribute::add_attribute(Vertex_Data_Type type, GLuint size, GLuint offset)
{
    glEnableVertexAttribArray(index);

    if (type == Vertex_Data_Type::FLOAT) {
        glVertexAttribPointer(index, size, GL_FLOAT, GL_FALSE, stride, (void *) offset);
    } else if (type == Vertex_Data_Type::INT) {
        glVertexAttribIPointer(index, size, GL_INT, stride, (void *) offset);
    } else {
        std::cerr << "Invalid vertex data type" << std::endl;
    }

    index++;
}
