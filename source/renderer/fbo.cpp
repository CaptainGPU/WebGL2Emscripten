#include "renderer/fbo.h"

Framebuffer::Framebuffer(int width, int height) :
m_width(width),
m_height(height),
m_fboID(0),
m_rboID(0)
{
    invalidate();
}

Framebuffer::~Framebuffer()
{
    glDeleteFramebuffers(1, &m_fboID);
    glDeleteRenderbuffers(1, &m_rboID);
}

void Framebuffer::invalidate()
{
    if (m_fboID) 
    {
        glDeleteFramebuffers(1, &m_fboID);
        glDeleteRenderbuffers(1, &m_fboID);
    }

    glGenFramebuffers(1, &m_fboID);
    glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);

    m_colorTexture = std::make_unique<Texture>(m_width, m_height, GL_RGB, GL_RGB);

    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, m_colorTexture->getID(), 0);

    glGenRenderbuffers(1, &m_rboID);
    glBindRenderbuffer(GL_RENDERBUFFER, m_rboID);
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, m_width, m_height);
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, m_rboID);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
    {
        printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    printf("FBO initialized/resized to: %dx%d\n", m_width, m_height);
}

void Framebuffer::bind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, m_fboID);
    glViewport(0, 0, m_width, m_height);
}

void Framebuffer::unbind() const
{
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
}

Texture* Framebuffer::getColorTexture()
{
    return m_colorTexture.get();
}

void Framebuffer::resize(int width, int height) 
{
    m_width = width;
    m_height = height;
    invalidate();
}