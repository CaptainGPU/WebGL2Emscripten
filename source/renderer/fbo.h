#pragma once

#include "renderer/texture.h"
#include <memory>

class Framebuffer
{
public:
    Framebuffer(int width, int height);
    ~Framebuffer();

    void bind() const;
    void unbind() const;

    Texture* getColorTexture();

    void resize(int width, int height);

private:
    GLuint m_fboID;
    GLuint m_rboID;
    std::unique_ptr<Texture> m_colorTexture;
    int m_width;
    int m_height;

    void invalidate();
};