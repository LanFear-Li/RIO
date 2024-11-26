#include "frame-buffer.hpp"

#include <iostream>

Frame_Buffer::Frame_Buffer(): active_id(0)
{
    glGenFramebuffers(1, &fbo_id);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
}

Frame_Buffer::Frame_Buffer(GLuint buffer_id): fbo_id(buffer_id), active_id(0)
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
}

Frame_Buffer::~Frame_Buffer()
{
    glDeleteFramebuffers(1, &fbo_id);
}

void Frame_Buffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, fbo_id);
}

void Frame_Buffer::unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

void Frame_Buffer::attach_texture(GLenum attachment, GLenum target, GLuint texture_id, GLuint mip) const
{
    glFramebufferTexture2D(GL_FRAMEBUFFER, attachment, target, texture_id, mip);
}

void Frame_Buffer::attach_render_buffer(GLenum attachment, GLuint render_buffer_id) const
{
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, attachment, GL_RENDERBUFFER, render_buffer_id);
}

void Frame_Buffer::bind_texture(GLenum target, GLuint shader_id, GLuint texture_id, const char *tex_name)
{
    glActiveTexture(GL_TEXTURE0 + active_id);
    glUniform1i(glGetUniformLocation(shader_id, tex_name), active_id);
    glBindTexture(target, texture_id);

    active_id++;
}

void Frame_Buffer::reset_active_id()
{
    glActiveTexture(GL_TEXTURE0);
    active_id = 0;
}
