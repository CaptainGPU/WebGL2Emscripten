#pragma once

#include "glm/fwd.hpp"
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

struct Transform
{
    glm::vec3 position = { 0.0f, 0.0f, 0.0f};
    glm::vec3 rotation = { 0.0f, 0.0f, 0.0f};
    glm::vec3 scale = { 0.0f, 0.0f, 0.0f};

    glm::mat4 getModelMatrix() const {
        glm::mat4 model = glm::mat4(1.0f);
        model = glm::translate(model, position);
        model = glm::rotate(model, glm::radians(rotation.x), {1, 0, 0});
        model = glm::rotate(model, glm::radians(rotation.y), {0, 1, 0});
        model = glm::rotate(model, glm::radians(rotation.z), {0, 0, 1});
        model = glm::scale(model, scale);
        return model;
    }
};