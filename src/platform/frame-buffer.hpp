#pragma once

#include "platform/texture.hpp"

#include <glad/glad.h>

class Frame_Buffer final
{
public:
    Frame_Buffer();
    Frame_Buffer(GLuint buffer_id);
    ~Frame_Buffer();

    void bind() const;
    void unbind() const;
    void attach_texture(GLenum attachment, GLenum target, GLuint texture_id, GLuint mip=0) const;
    void attach_render_buffer(GLenum attachment, GLuint render_buffer_id) const;

    void bind_texture(GLenum target, GLuint shader_id, GLuint texture_id, const char *tex_name);
    void reset_active_id();

private:
    GLuint active_id;
    GLuint fbo_id;
};
