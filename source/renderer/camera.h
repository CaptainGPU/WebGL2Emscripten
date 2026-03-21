#pragma once

#include "transform.h"

class Camera {
public:
    float fov = 45.0f;
    float nearPlane = 0.1f;
    float farPlane = 100.0f;

    Transform transform;

    glm::mat4 getViewMatrix() const {
        glm::vec3 front;
        front.x = cos(glm::radians(transform.rotation.y)) * cos(glm::radians(transform.rotation.x));
        front.y = sin(glm::radians(transform.rotation.x));
        front.z = sin(glm::radians(transform.rotation.y)) * cos(glm::radians(transform.rotation.x));
        front = glm::normalize(front);

        glm::vec3 worldUp = { 0.0f, 1.0f, 0.0f };

        return glm::lookAt(transform.position, transform.position + front, worldUp);
    }

    glm::mat4 getProjectionMatrix(float aspectRatio) const {
        return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    }
};