#pragma once

#include "core/windows.h"
#include "core/transform.h"
#include "scene/entity.h"
#include "renderer/camera.h"
#include "renderer/shader.h"
#include "renderer/mesh.h"
#include "renderer/texture.h"
#include "renderer/fbo.h"

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
    void initScreenQuads();
    void toggleFullscreen();

    std::unique_ptr<Shader> m_mainShader;
    std::unique_ptr<Shader> m_screenShader;

    std::unique_ptr<Mesh> m_ogreMesh;
    std::unique_ptr<Mesh> m_screenQuad;
    std::unique_ptr<Entity> m_ogre;
    std::unique_ptr<Texture> m_texture;
    std::unique_ptr<Framebuffer> m_fbo;

private:
    float m_worldTime = 0.0f;
    double m_lastTime = 0.0f;
    float m_rotationSpeed = 50.0f;
    int m_currentFBOWidth = 0;
    int m_currentFBOHeight = 0;
    glm::vec3 m_lightColor = glm::vec3(1.0f, 1.0f, 1.0f);
    bool m_useTexture = true;
    static constexpr int FPS_HISTORY_SIZE = 100;
    float m_fpsHistory[FPS_HISTORY_SIZE] = { 0 };
    int m_fpsOffset = 0;

private:
    std::vector<float> objVertices;
    std::vector<unsigned int> objIndices;

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