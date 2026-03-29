#pragma once

#include <vector>
#include <GLES3/gl3.h>

struct Vertex
{
    float position[3];
    float color[3];
    float uv[2];
    float normal[3];
};

class Mesh
{
public:
    Mesh(const std::vector<float>& rawVertices, const std::vector<unsigned int>& indices);
    ~Mesh();

    void draw() const;

private:
    GLuint m_vao, m_vbo, m_ebo;
    GLsizei m_indexCount;
};