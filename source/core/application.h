#pragma once

#include "core/windows.h"
#include "renderer/camera.h"
#include "renderer/transform.h"
#include "renderer/shader.h"
#include "renderer/mesh.h"

#include <emscripten.h>
#include <string>
#include <vector>

#include <GLES3/gl3.h>

class Application
{
public:
    Application();
    ~Application();

    Application(const Application&) = delete;
    Application& operator=(const Application&) = delete;

    void run();

private:
    bool init();
    void initScene();
    void update(float dt);
    void render();

    void initImGui();
    void loadOBJ(const std::string& path);
    GLuint createShaderProgram(const std::string& vertPath, const std::string& fragPath);
    std::string readFile(const std::string& filePath);
    void initScreenQuads();
    void initTexture(const char* texturePath);
    void initFBO(int width, int height);
    void toggleFullscreen();

    std::unique_ptr<Shader> m_mainShader;
    std::unique_ptr<Mesh> m_ogreMesh;

private:
    float m_worldTime = 0.0f;
    double m_lastTime = 0.0f;
    float m_rotationSpeed = 50.0f;
    int m_currentFBOWidth = 0;
    int m_currentFBOHeight = 0;
    glm::vec3 m_lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    bool m_useTexture = true;
    static constexpr int FPS_HISTORY_SIZE = 100; // Константа всередині класу
    float m_fpsHistory[FPS_HISTORY_SIZE] = { 0 };
    int m_fpsOffset = 0;

    GLuint fbo = 0;
    GLuint textureColorBuffer = 0;
    GLuint rbo = 0;


private:
    std::vector<float> objVertices;
    std::vector<unsigned int> objIndices;
    GLuint screenVAO, screenVBO;
    GLuint screenShaderProgram;

    GLuint textureID;


private:

    static void mainLoop(void* arg);

    static EM_BOOL onMouseCallback(int eventType, const EmscriptenMouseEvent *e, void *userData);
    static EM_BOOL onWheelCallback(int eventType, const EmscriptenWheelEvent *e, void *userData);
    static EM_BOOL onTouchCallback(int eventType, const EmscriptenTouchEvent *e, void *userData);

    EM_BOOL handleOnMouseCallback(int eventType, const EmscriptenMouseEvent *e);
    EM_BOOL handleOnWheelCallback(int eventType, const EmscriptenWheelEvent *e);
    EM_BOOL handleOnTouchCallback(int eventType, const EmscriptenTouchEvent *e);

    

    Window      m_window;
    Camera      m_camera;
    Transform   m_ogreTransform;
    bool        m_isRunning = false;
};