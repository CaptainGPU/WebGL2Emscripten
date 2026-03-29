#include "scene/entity.h"

Entity::Entity(Mesh* mesh, Shader* shader, Texture* texture) :
m_mesh(mesh),
m_shader(shader),
m_texture(texture)
{
    transform.scale = {1.0f, 1.0f, 1.0f};
    transform.position = {0.0f, 0.0f, 0.0f};
}

void Entity::draw(
    const glm::mat4& view, const glm::mat4& projection
)
{
    if (!m_mesh || !m_shader) 
    {
        return;
    }

    m_shader->use();
    m_texture->bind(0);

    const glm::mat4& modelMatrix = transform.getModelMatrix();

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(modelMatrix)));

    m_shader->setMat4("uModel", modelMatrix);
    m_shader->setMat4("uView", view);
    m_shader->setMat4("uProjection", projection);
    m_shader->setMat3("uNormalMatrix", normalMatrix);

    m_shader->setInt("uTexture", 0);
    m_shader->setInt("uUseTexture", 1);

    m_shader->setVec3("uLightPos", glm::vec3(5.0f, 5.0f, 3.0f));
    m_shader->setVec3("uLightColor", glm::vec3(1.0f, 1.0f, 1.0f));

    m_mesh->draw();
}