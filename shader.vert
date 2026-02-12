#version 300 es
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aColor;
layout (location = 2) in vec2 aTexCoord;

uniform float uTime;
uniform mat4 uModel;
uniform mat4 uProjection;

out vec3 ourColor;
out vec2 TexCoord;

void main() {
    float s = sin(uTime);
    float c = cos(uTime);
    
    mat3 rotY = mat3(c, 0, s, 0, 1, 0, -s, 0, c);
    mat3 rotX = mat3(1, 0, 0, 0, c, -s, 0, s, c);
    
    vec3 rotatedPos = rotX * (rotY * aPos);
    
    gl_Position = uProjection * uModel * vec4(rotatedPos, 1.0);
    ourColor = aColor;
    TexCoord = aTexCoord;
}