#version 300 es
precision highp float;

in vec3 ourColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D uTexture;
uniform vec3 uLightPos;
uniform vec3 uLightColor;

out vec4 FragColor;

void main() {
    
    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * uLightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(uLightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;

    vec3 result = (ambient + diffuse) * texture(uTexture, TexCoord).rgb;
    FragColor = vec4(result, 1.0);
}