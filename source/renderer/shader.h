#pragma once

#include <string>
#include <unordered_map>
#include "glm/glm.hpp"
#include <GLES3/gl3.h>

class Shader {
public:
    Shader(const std::string& vertPath, const std::string fragPath);
    ~Shader();

    void use() const;

    void setInt(const std::string& name, int value);
    void setFloat(const std::string& name, float value);
    void setVec3(const std::string& name, const glm::vec3& value);
    void setMat3(const std::string& name, const glm::mat3& value);
    void setMat4(const std::string& name, const glm::mat4& value);

private:

    GLuint getUniformLocation(const std::string& name);
    std::string readFile(const std::string& path);
    GLuint compileShader(GLenum type, const std::string& source);

    GLuint m_id;
    std::unordered_map<std::string, GLuint> m_uniformLocationCache;
};