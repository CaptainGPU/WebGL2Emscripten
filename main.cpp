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

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

std::vector<float> objVertices;
std::vector<unsigned int> objIndices;

void loadOBJ(const std::string& path) {
    tinyobj::attrib_t attrib;
    std::vector<tinyobj::shape_t> shapes;
    std::vector<tinyobj::material_t> materials;
    std::string warn, err;

    if (!tinyobj::LoadObj(&attrib, &shapes, &materials, &warn, &err, path.c_str())) {
        printf("Error OBJ: %s\n", err.c_str());
        return;
    }

    for (const auto& shape : shapes) {
        for (const auto& index : shape.mesh.indices) {
            // Позиції (x, y, z)
            objVertices.push_back(attrib.vertices[3 * index.vertex_index + 0]);
            objVertices.push_back(attrib.vertices[3 * index.vertex_index + 1]);
            objVertices.push_back(attrib.vertices[3 * index.vertex_index + 2]);

            objVertices.push_back(1.0f); objVertices.push_back(1.0f); objVertices.push_back(1.0f);

            if (index.texcoord_index >= 0) {
                objVertices.push_back(attrib.texcoords[2 * index.texcoord_index + 0]);
                objVertices.push_back(attrib.texcoords[2 * index.texcoord_index + 1]);
            } 
            else 
            {
                objVertices.push_back(0.0f); objVertices.push_back(0.0f);
            }

            if (index.normal_index >= 0) 
            {
                objVertices.push_back(attrib.normals[3 * index.normal_index + 0]);
                objVertices.push_back(attrib.normals[3 * index.normal_index + 1]);
                objVertices.push_back(attrib.normals[3 * index.normal_index + 2]);
            } 
            else 
            {
                objVertices.push_back(0.0f); 
                objVertices.push_back(1.0f); 
                objVertices.push_back(0.0f);
            }
            
            objIndices.push_back(objIndices.size());
        }
    }
    printf("Model loaded! Vertices: %zu\n", objVertices.size() / 8);
}

std::string readFile(const std::string& filePath) 
{
    std::ifstream file(filePath);
    if (!file.is_open()) 
    {
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

void initTexture(const char* texturePath) {
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    int width, height, nrChannels;

    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(texturePath, &width, &height, &nrChannels, 0);

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

GLuint createShaderProgram(const std::string& vertPath, const std::string& fragPath) 
{
    std::string vertexCode = readFile(vertPath);
    std::string fragmentCode = readFile(fragPath);

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

    GLuint program = glCreateProgram();
    glAttachShader(program, vertexShader);
    glAttachShader(program, fragmentShader);
    glLinkProgram(program);

    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    return program;
}

void initBuffers()
{
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    glGenBuffers(1, &EBO);

    glBindVertexArray(VAO);

    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, objVertices.size() * sizeof(float), objVertices.data(), GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, objIndices.size() * sizeof(unsigned int), objIndices.data(), GL_STATIC_DRAW);

    GLsizei stride = 11 * sizeof(float);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, stride, (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, stride, (void*)(6 * sizeof(float)));
    glEnableVertexAttribArray(2);

    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, stride, (void*)(8 * sizeof(float)));
    glEnableVertexAttribArray(3);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);
}

GLuint fbo = 0;
GLuint textureColorBuffer = 0;
GLuint rbo = 0;
int currentFBOWidth = 0;
int currentFBOHeight = 0;

void initFBO(int width, int height) {

    if (fbo != 0) {
        glDeleteFramebuffers(1, &fbo);
        glDeleteTextures(1, &textureColorBuffer);
        glDeleteRenderbuffers(1, &rbo);
    }

    glGenFramebuffers(1, &fbo);
    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glGenTextures(1, &textureColorBuffer);
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, textureColorBuffer, 0);

    glGenRenderbuffers(1, &rbo);
    glBindRenderbuffer(GL_RENDERBUFFER, rbo);
    
    glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH_COMPONENT24, width, height);
    
    glFramebufferRenderbuffer(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_RENDERBUFFER, rbo);

    if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
        printf("ERROR::FRAMEBUFFER:: Framebuffer is not complete!\n");

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    currentFBOWidth = width;
    currentFBOHeight = height;
    printf("FBO initialized/resized to: %dx%d\n", width, height);
}

GLuint screenVAO, screenVBO;
GLuint screenShaderProgram;

void initScreenQuads() {
    float quadVertices[] = {
        -1.0f,  1.0f,  0.0f, 1.0f,
        -1.0f, -1.0f,  0.0f, 0.0f,
         1.0f, -1.0f,  1.0f, 0.0f,

        -1.0f,  1.0f,  0.0f, 1.0f,
         1.0f, -1.0f,  1.0f, 0.0f,
         1.0f,  1.0f,  1.0f, 1.0f
    };

    glGenVertexArrays(1, &screenVAO);
    glGenBuffers(1, &screenVBO);
    glBindVertexArray(screenVAO);
    glBindBuffer(GL_ARRAY_BUFFER, screenVBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(quadVertices), &quadVertices, GL_STATIC_DRAW);

    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 4 * sizeof(float), (void*)(2 * sizeof(float)));
}

float worldTime = .0;

glm::vec3 cameraPos   = glm::vec3(0.0f, 0.0f, 3.0f);
glm::vec3 cameraFront = glm::vec3(0.0f, 0.0f, -1.0f);
glm::vec3 cameraUp    = glm::vec3(0.0f, 1.0f, 0.0f);

EM_BOOL render_frame(double time, void* userdata)
{
    worldTime = (float)time / 1000.0f;

    double cssWidth, cssHeight;
    emscripten_get_element_css_size("#canvas", &cssWidth, &cssHeight);

    double pixelRatio = emscripten_get_device_pixel_ratio();

    int canvasW = (int)(cssWidth * pixelRatio);
    int canvasH = (int)(cssHeight * pixelRatio);

    emscripten_set_canvas_element_size("#canvas", canvasW, canvasH);

    if (canvasW != currentFBOWidth || canvasH != currentFBOHeight) {
        initFBO(canvasW, canvasH);
    }

    // FIRST PASS

    glEnable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glViewport(0, 0, canvasW, canvasH);

    glm::vec3 color = glm::vec3(0.25f, 0.5f, 1.0f) * 0.25f;

    glClearColor(color.r, color.g, color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    glUseProgram(shaderProgram);

    glActiveTexture(GL_TEXTURE0);
    glBindTexture(GL_TEXTURE_2D, textureID);

    float aspect = (float)canvasW / (float)canvasH;
    glm::mat4 projection = glm::perspective(glm::radians(45.0f), aspect, 0.1f, 100.0f);

    glm::mat4 model = glm::mat4(1.0f);
    model = glm::rotate(model, worldTime, glm::vec3(0.0f, 1.0f, 0.0f));

    glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraFront, cameraUp);

    glm::mat3 normalMatrix = glm::transpose(glm::inverse(glm::mat3(model)));

    int modelLoc = glGetUniformLocation(shaderProgram, "uModel");
    int viewLoc = glGetUniformLocation(shaderProgram, "uView");
    int projLoc = glGetUniformLocation(shaderProgram, "uProjection");
    int timeLoc = glGetUniformLocation(shaderProgram, "uTime");
    int normalMatrixLoc  = glGetUniformLocation(shaderProgram, "uNormalMatrix");

    int lightPosLoc = glGetUniformLocation(shaderProgram, "uLightPos");
    int lightColLoc = glGetUniformLocation(shaderProgram, "uLightColor");

    glUniform1i(glGetUniformLocation(shaderProgram, "uTexture"), 0);

    glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));
    glUniformMatrix4fv(viewLoc, 1, GL_FALSE, glm::value_ptr(view));
    glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
    glUniformMatrix3fv(normalMatrixLoc, 1, GL_FALSE, glm::value_ptr(normalMatrix));
    glUniform1f(timeLoc, worldTime);

    glUniform3f(lightPosLoc, 5.0f, 5.0f, 3.0f);
    glUniform3f(lightColLoc, 1.0f, 1.0f, 1.0f);
    
    int indexCount = sizeof(indices) / sizeof(indices[0]);
    
    glBindVertexArray(VAO);
    glDrawElements(GL_TRIANGLES, objIndices.size(), GL_UNSIGNED_INT, 0);

    // SCREEN PASS

    glDisable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);

    glUseProgram(screenShaderProgram);
    glBindVertexArray(screenVAO);
    glUniform1i(glGetUniformLocation(screenShaderProgram, "screenTexture"), 0);
    glActiveTexture(GL_TEXTURE0); 
    glBindTexture(GL_TEXTURE_2D, textureColorBuffer);
    glDrawArrays(GL_TRIANGLES, 0, 6);

    return true;
}

int main()
{
    printf("Start test WebGL 2\n");

    EmscriptenWebGLContextAttributes attr;
    attr.majorVersion = 2;
    attr.minorVersion = 0;
    attr.depth = EM_TRUE;

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("#canvas", &attr);

    if (ctx <= 0)
    {
        printf("Error: WebGL 2 not supported!");
        return 1;
    }

    emscripten_webgl_make_context_current(ctx);

    glEnable(GL_DEPTH_TEST);
    //glEnable(GL_CULL_FACE); 
    glCullFace(GL_BACK);

    const GLubyte* renderer = glGetString(GL_RENDERER);
    const GLubyte* version = glGetString(GL_VERSION);
    const GLubyte* glslVersion = glGetString(GL_SHADING_LANGUAGE_VERSION);

    printf("Videocard: %s\n", renderer);
    printf("GL Version: %s \n", version);
    printf("GLSL Version: %s\n", glslVersion);

    printf("WebGL initialized!!!\n");

    loadOBJ("ogre.geom");
    shaderProgram = createShaderProgram("shader.vert", "shader.frag");
    screenShaderProgram = createShaderProgram("screen_shader.vert", "screen_shader.frag");
    initBuffers();
    initScreenQuads();
    initTexture("ogre.png");

    emscripten_request_animation_frame_loop(render_frame, nullptr);

    return 0;
}