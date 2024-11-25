#include "render-buffer.hpp"

Render_Buffer::Render_Buffer(int width, int height, GLenum buffer_format)
    : rbo_width(width), rbo_height(height) {
    glGenRenderbuffers(1, &rbo_id);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_id);
    glRenderbufferStorage(GL_RENDERBUFFER, buffer_format, width, height);
}

Render_Buffer::~Render_Buffer() {
    glDeleteRenderbuffers(1, &rbo_id);
}

void Render_Buffer::bind() const {
    glBindRenderbuffer(GL_RENDERBUFFER, rbo_id);
}

void Render_Buffer::unbind() const {
    glBindRenderbuffer(GL_RENDERBUFFER, 0);
}
