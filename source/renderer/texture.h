#pragma once

#include <string>
#include <GLES3/gl3.h>

class Texture
{
public:
    Texture(const std::string& path);
    Texture(int width, int height, GLint internalFormat = GL_RGB, GLenum format = GL_RGB);
    ~Texture();

    void bind(unsigned int unit = 0) const;
    void unbind() const;

    GLuint getID() const;

private:
    GLuint m_id;
    int m_width;
    int m_height;
};