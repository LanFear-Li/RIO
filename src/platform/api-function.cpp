#include "api-function.hpp"

void Api_Function::set_viewport(int width, int height)
{
    glViewport(0, 0, width, height);
}

void Api_Function::set_clear_color(const glm::vec4 &color)
{
    glClearColor(color.r, color.g, color.b, color.a);
}

void Api_Function::clear_depth()
{
    glClear(GL_DEPTH_BUFFER_BIT);
}

void Api_Function::clear_color()
{
    glClear(GL_COLOR_BUFFER_BIT);
}

void Api_Function::clear_color_and_depth()
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
}

void Api_Function::draw_arrays(GLenum mode, int first, int count)
{
    glDrawArrays(mode, first, count);
}

void Api_Function::draw_elements(GLenum mode, int count, GLenum type, const void *indices)
{
    glDrawElements(mode, count, type, indices);
}
