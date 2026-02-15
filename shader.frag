#version 300 es
precision highp float;

in vec3 ourColor;
in vec2 TexCoord;

uniform sampler2D uTexture;

out vec4 FragColor;

void main() {
    vec2 flippedTexCoord = TexCoord;//vec2(1.0 - TexCoord.x, TexCoord.y);
    FragColor = texture(uTexture, flippedTexCoord);
}