#pragma once

#include <glad/glad.h>

enum Vertex_Data_Type
{
    FLOAT, INT, BOOL
};

class Vertex_Attribute final
{
public:
    Vertex_Attribute() = delete;
    Vertex_Attribute(GLsizei stride);

    void add_attribute(Vertex_Data_Type type, GLuint size, GLuint offset);

private:
    GLuint index;
    GLsizei stride;

};
