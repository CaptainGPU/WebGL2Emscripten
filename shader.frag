#version 300 es
precision highp float;

in vec3 ourColor;
in vec2 TexCoord;
in vec3 Normal;
in vec3 FragPos;

uniform sampler2D uTexture;
uniform vec3 uLightPos;
uniform vec3 uLightColor;
uniform bool uUseTexture;

out vec4 FragColor;

void main() {
    
    vec3 baseColor = vec3(0.7);
    if (uUseTexture) 
    {
        baseColor = texture(uTexture, TexCoord).rgb;
    }

    float ambientStrength = 0.2;
    vec3 ambient = ambientStrength * uLightColor;

    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(uLightPos - FragPos);

    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * uLightColor;

    vec3 result = (ambient + diffuse) * baseColor;
    FragColor = vec4(result, 1.0);
}