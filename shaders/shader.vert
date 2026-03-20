#version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aNormal;

uniform float uTime;
uniform mat4 uModel;
uniform mat4 uProjection;
uniform mat4 uView;
uniform mat3 uNormalMatrix;

out vec3 ourColor;
out vec2 TexCoord;
out vec3 Normal;
out vec3 FragPos;

void main() 
{

    FragPos = vec3(uModel * vec4(aPos, 1.0));
    Normal = uNormalMatrix * aNormal;

    ourColor = aColor;
    TexCoord = aTexCoord;

    gl_Position = uProjection * uView * uModel * vec4(aPos, 1);

    gl_Position = uProjection * uView * vec4(FragPos, 1.0);
}