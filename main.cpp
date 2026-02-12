#include <iostream>
#include <emscripten.h>
#include <emscripten/html5.h>
#include <GLES3/gl3.h>

#include <fstream>
#include <sstream>
#include <string>
#include <vector>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

std::string readFile(const std::string& filePath) {
    std::ifstream file(filePath);
    if (!file.is_open()) {
        printf("FAILED to open file: %s\n", filePath.c_str());
        return "";
    }
    std::stringstream buffer;
    buffer << file.rdbuf();
    return buffer.str();
}

float vertices[] = {
    
    -0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,

    
    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  1.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  0.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  0.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  1.0f, 1.0f,

     0.5f, -0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,

    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f,

    -0.5f, -0.5f, -0.5f,  1.0f, 0.0f, 0.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f, 0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f, 1.0f,  1.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  1.0f, 1.0f, 1.0f,  0.0f, 1.0f 
};

unsigned int indices[] = {
    0, 1, 2,   2, 3, 0,      
    4, 7, 6,   6, 5, 4,      
    8, 9, 10,  10, 11, 8,    
    12, 13, 14, 14, 15, 12,  
    16, 17, 18, 18, 19, 16,  
    20, 21, 22, 22, 23, 20   
};

GLuint shaderProgram;
GLuint VAO, VBO, EBO, textureID;

// void initTexture() {
//     glGenTextures(1, &textureID);
//     glBindTexture(GL_TEXTURE_2D, textureID);

//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);

//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
//     glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);


//     GLubyte pixels[] = {
//         255, 255, 255, 255,   0,   0,   0, 255, // Білий, Чорний
//           0,   0,   0, 255, 255, 255, 255, 255  // Чорний, Білий
//     };

//     glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, 2, 2, 0, GL_RGBA, GL_UNSIGNED_BYTE, pixels);
// }

void initTexture() {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    int width, height, nrChannels;

    unsigned char *data = stbi_load("texture.png", &width, &height, &nrChannels, 0);

    if (data) {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
        printf("Texture loaded successfully: %dx%d\n", width, height);
    } else {
        printf("Failed to load texture. Reason: %s\n", stbi_failure_reason());
    }
}

void initShaders() {
    std::string vertexCode = readFile("shader.vert");
    std::string fragmentCode = readFile("shader.frag");

    const char* vShaderCode = vertexCode.c_str();
    const char* fShaderCode = fragmentCode.c_str();

    GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
    glShaderSource(vertexShader, 1, &vShaderCode, nullptr);
    glCompileShader(vertexShader);
    
    GLint success;
    GLchar infoLog[512];
    glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(vertexShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::VERTEX::COMPILATION_FAILED\n%s\n", infoLog);
    }

    GLuint fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fShaderCode, nullptr);
    glCompileShader(fragmentShader);
    
    glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(fragmentShader, 512, NULL, infoLog);
        printf("ERROR::SHADER::FRAGMENT::COMPILATION_FAILED\n%s\n", infoLog);
    }

    shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);
}

void initBuffers()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    GLsizei stride = 8 * sizeof(float);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

void set_projection_matrix(int width, int height)
{
    float aspect = (float)width / float(height);
    float matrix[16] = {0};

    matrix[0] = 1.0f;
    matrix[5] = 1.0f;
    matrix[10] = 1.0f;
    matrix[15] = 1.0f;

    if (width >= height)
    {
        matrix[0] = 1.0f / aspect;
    }
    else
    {
        matrix[5] = aspect;
    }

    int projLoc = glGetUniformLocation(shaderProgram, "uProjection");
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, matrix);
}

void set_identity(float* m) 
{
    for(int i=0; i<16; i++) m[i] = 0.0f;
    m[0] = m[5] = m[10] = m[15] = 1.0f;
}

void set_rotate_model(float* m, float time) {
    set_identity(m);
    
    float s1 = sin(time);
    float c1 = cos(time);
    float s2 = sin(time * 0.5f);
    float c2 = cos(time * 0.5f);

    m[0] = c1;
    m[2] = -s1;
    
    m[5] = c2;
    m[6] = s2;
    
    m[8] = s1 * c2;
    m[9] = -s2;
    m[10] = c1 * c2;

    m[14] = -3.0f;
}

void set_ortho_projection(float* m, int width, int height) {
    set_identity(m);
    float aspect = (float)width / (float)height;
    if (width >= height) {
        m[0] = 1.0f / aspect;
    } else {
        m[5] = aspect;
    }
}

void set_perspective_projection(float* m, int width, int height) {
    float fov = 45.0f * (3.14159265f / 180.0f); 
    float aspect = (float)width / (float)height;
    float near = 0.1f;
    float far = 100.0f;

    float f = 1.0f / tan(fov / 2.0f);

    set_identity(m);

    m[0] = f / aspect;
    m[5] = f;
    m[10] = (far + near) / (near - far);
    m[11] = -1.0f; 
    m[14] = (2.0f * far * near) / (near - far);
    m[15] = 0.0f;
}

void getPerspectiveMatrix(float fov, float aspect, float near, float far, float* matrix) {
    float f = 1.0f / tan(fov / 2.0f);
    for(int i = 0; i < 16; i++) matrix[i] = 0.0f;
    matrix[0] = f / aspect;
    matrix[5] = f;
    matrix[10] = (far + near) / (near - far);
    matrix[11] = -1.0f;
    matrix[14] = (2.0f * far * near) / (near - far);
}

float worldTime = .0;

EM_BOOL render_frame(double time, void* userdata)
{
    worldTime = (float)time / 1000.0f;

    double width, height;
    emscripten_get_element_css_size("#canvas", &width, &height);

    double pixelRatio = emscripten_get_device_pixel_ratio();

    int canvasW = (int)(width * pixelRatio);
    int canvasH = (int)(height * pixelRatio);

    emscripten_set_canvas_element_size("#canvas", canvasW, canvasH);

    glViewport(0, 0, canvasW, canvasH);

    glClearColor(0.25f, 0.5f, 1.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    float projection[16];
    float t = (float)time / 1000.0f;

    
    float model[16] = {
        1, 0, 0, 0,
        0, 1, 0, 0,
        0, 0, 1, 0,
        0, 0, -3, 1  // Z = -3
    };

    getPerspectiveMatrix(0.78f, (float)canvasW/canvasH, 0.1f, 100.0f, projection);

    int modelLoc = glGetUniformLocation(shaderProgram, "uModel");
    int projLoc = glGetUniformLocation(shaderProgram, "uProjection");
    int timeLoc = glGetUniformLocation(shaderProgram, "uTime");

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, model);
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, projection);
    glUniform1f(timeLoc, worldTime);
    
    int indexCount = sizeof(indices) / sizeof(indices[0]);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, 0);

    return true;
}

int main()
{
    printf("Start test WebGL 2\n");

    EmscriptenWebGLContextAttributes attr;
    attr.majorVersion = 2;
    attr.minorVersion = 0;

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("#canvas", &attr);

    if (ctx <= 0)
    {
        printf("Error: WebGL 2 not supported!");
        return 1;
    }

    emscripten_webgl_make_context_current(ctx);

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_CULL_FACE); 
    glCullFace(GL_BACK);

    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("Videocard: %s\n", renderer);
    printf("GL Version: %s \n", version);
    printf("GLSL Version: %s\n", glslVersion);

    printf("WebGL initialized!!!\n");

    initShaders();
    initBuffers();
    initTexture();

    emscripten_request_animation_frame_loop(render_frame, nullptr);
    
    return 0;
}