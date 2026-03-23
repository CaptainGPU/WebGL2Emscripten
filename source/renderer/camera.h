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

        float yawRad   = glm::radians(transform.rotation.y);
        float pitchRad = glm::radians(transform.rotation.x);

        front.x = cos(yawRad) * cos(pitchRad);
        front.y = sin(pitchRad);
        front.z = sin(yawRad) * cos(pitchRad);

        front = glm::normalize(front);

        glm::vec3 worldUp = { 0.0f, 1.0f, 0.0f };

        return glm::lookAt(transform.position, transform.position + front, worldUp);
        //return glm::translate(glm::mat4(1.0f), glm::vec3(0, 0, -3.0f));
    }

    glm::mat4 getProjectionMatrix(float aspectRatio) const {
        return glm::perspective(glm::radians(fov), aspectRatio, nearPlane, farPlane);
    }
};