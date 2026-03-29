#pragma once

#include "renderer/mesh.h"
#include "renderer/shader.h"
#include "renderer/texture.h"
#include "core/transform.h"

class Entity
{
public:
    Entity(Mesh* mesh, Shader* shader, Texture* texture);

    Transform transform;

    void draw(
        const glm::mat4& view, 
        const glm::mat4& projection
    );

private:
    Mesh* m_mesh;
    Shader* m_shader;
    Texture* m_texture;
};