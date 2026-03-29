#include "core/application.h"
#include <cstdio>

#include "imgui.h"
#include "imgui_impl_opengl3.h"

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define TINYOBJLOADER_IMPLEMENTATION
#include "tiny_obj_loader.h"

#include <fstream>
#include <sstream>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#ifdef NDEBUG
    const char* buildType = "Release";
#else
    const char* buildType = "Debug";
#endif

#define STR_HELPER(x) #x
#define STR(x) STR_HELPER(x)
const char* emVersion = STR(__EMSCRIPTEN_MAJOR__) "." STR( __EMSCRIPTEN_MINOR__) "." STR(__EMSCRIPTEN_TINY__);

Application::Application()
{

}

Application::~Application()
{

}

void Application::run()
{
    if (!init()) 
    {
        printf("Application: Initialization Error \n");
        return;
    }


    m_isRunning = true;

    emscripten_set_main_loop_arg(Application::mainLoop, this, 0, 1);
}

bool Application::init()
{
    printf("Application: Initialization\n");

    if (!m_window.init("#canvas"))
    {
        printf("Window: Initization error\n");
        return false;
    }

    printf("Start test WebGL 2\n");

    EmscriptenWebGLContextAttributes attr;
    emscripten_webgl_init_context_attributes(&attr);
    attr.majorVersion = 2;
    attr.minorVersion = 0;
    attr.depth = EM_TRUE;
    printf("start emscripten_webgl_create_context\n");

    EMSCRIPTEN_WEBGL_CONTEXT_HANDLE ctx = emscripten_webgl_create_context("#canvas", &attr);

    if (ctx <= 0)
    {
        printf("Error: WebGL 2 not supported!");
        return 1;
    }

    printf("end emscripten_webgl_create_context\n");

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

    initImGui();

    loadOBJ("ogre.geom");
    initScreenQuads();

    m_currentFBOWidth = m_window.getWidth();
    m_currentFBOHeight = m_window.getHeight();

    m_fbo = std::make_unique<Framebuffer>(m_currentFBOWidth, m_currentFBOHeight);

    m_mainShader = std::make_unique<Shader>("shader.vert", "shader.frag");
    m_screenShader = std::make_unique<Shader>("screen_shader.vert", "screen_shader.frag");

    m_ogreMesh = std::make_unique<Mesh>(objVertices, objIndices);
    m_texture = std::make_unique<Texture>("ogre.png");

    m_ogre = std::make_unique<Entity>(m_ogreMesh.get(), m_mainShader.get(), m_texture.get());

    initScene();


    printf("Application: Initialization complete\n");

    return true;
}

void Application::initScene()
{
    m_camera.transform.position = {0.0f, 0.0f, 5.0f};
    m_camera.transform.rotation = {0.0f, -90.0f, 0.0f};
}

void Application::update(float dt)
{
    float currentFPS = (dt > 0) ? 1.0f / dt : 0.0f;

    m_fpsHistory[m_fpsOffset] = currentFPS;
    m_fpsOffset = (m_fpsOffset + 1) % FPS_HISTORY_SIZE;

    m_ogre->transform.rotation.y += m_rotationSpeed * dt;
}

void Application::render()
{
    int cssWidth = m_window.getWidth();
    int cssHeight = m_window.getHeight();

    double pixelRatio = m_window.getDevicePixelRatio();

    int canvasW = m_window.getFramebufferWidth();
    int canvasH = m_window.getFramebufferHeight();

    if (canvasW != m_currentFBOWidth || canvasH != m_currentFBOHeight) 
    {
        m_currentFBOWidth = canvasW;
        m_currentFBOHeight = canvasH;

        m_fbo->resize(m_currentFBOWidth, m_currentFBOHeight);
    }


    // FIRST PASS

    glEnable(GL_DEPTH_TEST);

    m_fbo->bind();

    glViewport(0, 0, canvasW, canvasH);

    glm::vec3 color = glm::vec3(0.25f, 0.5f, 1.0f) * 0.25f;

    glClearColor(color.r, color.g, color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspect = m_window.getAspectRatio();
    glm::mat4 projection = m_camera.getProjectionMatrix(aspect);
    glm::mat4 view = m_camera.getViewMatrix();

    m_ogre->draw(view, projection);

    m_fbo->unbind();

    // SCREEN PASS

    glDisable(GL_DEPTH_TEST);

    glClearColor(1.0f, 0.0f, 0.0f, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT);


    m_screenShader->use();

    m_screenShader->setInt("screenTexture", 0);
    m_fbo->getColorTexture()->bind(0);
    m_screenQuad->draw();

    // IMGUI PASS

    ImGui_ImplOpenGL3_NewFrame();

    ImGuiIO& io = ImGui::GetIO();
    io.DisplayFramebufferScale = ImVec2(pixelRatio, pixelRatio);
    io.DisplaySize = ImVec2(cssWidth, cssHeight);
    io.DeltaTime = 1.0f / 60.0f;

    ImGui::NewFrame();

    float panelWidth = 300.0f;
    ImGui::SetNextWindowPos(ImVec2(0, 0));
    ImGui::SetNextWindowSize(ImVec2(panelWidth, io.DisplaySize.y));

    ImGuiWindowFlags window_flags = 0;
    window_flags |= ImGuiWindowFlags_NoTitleBar;
    window_flags |= ImGuiWindowFlags_NoCollapse;
    window_flags |= ImGuiWindowFlags_NoResize;
    window_flags |= ImGuiWindowFlags_NoMove;
    window_flags |= ImGuiWindowFlags_NoBringToFrontOnFocus;

    ImGui::Begin("Control Panel", nullptr, window_flags);

    if (ImGui::CollapsingHeader("Screen Metrics", ImGuiTreeNodeFlags_DefaultOpen)) 
    {
        ImGui::Text("Pixel Ratio: "); 
        ImGui::SameLine(); 
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%.2f", pixelRatio);
        ImGui::Text("Render Buffer: %d x %d px", canvasW, canvasH);
        ImGui::Text("CSS Size:      %.1f x %.1f pts", (float)cssWidth, (float)cssHeight);
        ImGui::Text("ImGui Display: %.1f x %.1f", io.DisplaySize.x, io.DisplaySize.y);
    }

    ImGui::Separator();
    if (ImGui::CollapsingHeader("Build & Engine Info", ImGuiTreeNodeFlags_DefaultOpen)) 
    {
        ImGui::Text("Emscripten:");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(0.4f, 0.7f, 1.0f, 1.0f), "%s", emVersion);

        ImGui::Text("Build Configuration:");
        ImGui::SameLine();
        
    #ifdef NDEBUG
        ImGui::TextColored(ImVec4(0.0f, 1.0f, 0.0f, 1.0f), "%s", buildType);
    #else
        ImGui::TextColored(ImVec4(1.0f, 0.4f, 0.4f, 1.0f), "%s", buildType);
    #endif

        ImGui::Text("Graphics API:");
        ImGui::SameLine();
        ImGui::TextColored(ImVec4(1.0f, 0.8f, 0.0f, 1.0f), "WebGL 2.0 (ES 3.0)");

        ImGui::Text("Build Date: %s", __DATE__);
    }
    
    ImGui::SliderFloat("Rotation Speed", &m_rotationSpeed, 0.0f, 100.0f);

    ImGui::Separator();
    EmscriptenFullscreenChangeEvent fsStatus;
    emscripten_get_fullscreen_status(&fsStatus);

    const char* buttonLabel = fsStatus.isFullscreen ? "Exit Fullscreen" : "Go Fullscreen";

    if (ImGui::Button(buttonLabel, ImVec2(-1, 40))) \
    {
        toggleFullscreen();
    }

    if (ImGui::IsItemHovered()) 
    {
        ImGui::SetTooltip("Tip: Press Esc to exit fullscreen");
    }

    ImGui::Separator();
    ImGui::Text("Performance:");

    float avg = 0;
    for (int n = 0; n < FPS_HISTORY_SIZE; n++) avg += m_fpsHistory[n];
    avg /= (float)FPS_HISTORY_SIZE;

    char overlay[32];
    sprintf(overlay, "Avg: %.2f FPS", avg);

    ImGui::PlotLines("##fps", m_fpsHistory, FPS_HISTORY_SIZE, m_fpsOffset, overlay, 0.0f, 120.0f, ImVec2(0, 80.0f));


    ImGui::End();

    ImGui::Render();
    ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
}

void Application::initImGui()
{
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.IniFilename = nullptr;
    
    io.Fonts->AddFontDefault(0);

    ImGui::StyleColorsDark();

    ImGuiStyle& style = ImGui::GetStyle();
    style.WindowPadding = ImVec2(15, 15);
    style.FramePadding = ImVec2(10, 8);
    style.ItemSpacing = ImVec2(12, 10);
    style.FrameRounding = 5.0f;

    ImGui_ImplOpenGL3_Init("#version 300 es");

    emscripten_set_mousedown_callback("#canvas", this, EM_FALSE, Application::onMouseCallback);
    emscripten_set_mouseup_callback("#canvas", this, EM_FALSE, Application::onMouseCallback);
    emscripten_set_mousemove_callback("#canvas", this, EM_FALSE, Application::onMouseCallback);
    emscripten_set_wheel_callback("#canvas", this, EM_FALSE, Application::onWheelCallback);
    emscripten_set_touchstart_callback("#canvas", this, EM_FALSE, Application::onTouchCallback);
    emscripten_set_touchend_callback("#canvas", this, EM_FALSE, Application::onTouchCallback);
    emscripten_set_touchmove_callback("#canvas", this, EM_FALSE, Application::onTouchCallback);
    emscripten_set_touchcancel_callback("#canvas", this, EM_FALSE, Application::onTouchCallback);
}

void Application::loadOBJ(const std::string& path)
{
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

void Application::initScreenQuads()
{
    std::vector<float> quadVertices = {
        -1.0f,  1.0f, 0.0f,  0, 0, 0,  0.0f, 1.0f,  0, 0, 1,
        -1.0f, -1.0f, 0.0f,  0, 0, 0,  0.0f, 0.0f,  0, 0, 1,
         1.0f, -1.0f, 0.0f,  0, 0, 0,  1.0f, 0.0f,  0, 0, 1,
         1.0f,  1.0f, 0.0f,  0, 0, 0,  1.0f, 1.0f,  0, 0, 1
    };

    std::vector<unsigned int> quadIndices = {
        0, 1, 2,
        0, 2, 3
    };

    m_screenQuad = std::make_unique<Mesh>(quadVertices, quadIndices);
}

void Application::toggleFullscreen()
{
    EmscriptenFullscreenChangeEvent status;
    emscripten_get_fullscreen_status(&status);

    EMSCRIPTEN_RESULT result = EMSCRIPTEN_RESULT_SUCCESS;;

    if (status.isFullscreen)
    {
        printf("Exit from fullscreen\n");
        result = emscripten_exit_fullscreen();
    }
    else
    {
        printf("Go to fullscreen\n");
        result = emscripten_request_fullscreen("#canvas", EM_TRUE);
    }

    if (result != EMSCRIPTEN_RESULT_SUCCESS) 
    {
        printf("Fullscreen request failed with code: %d\n", result);
    }
}

void Application::mainLoop(void* arg) 
{
    Application* app = static_cast<Application*>(arg);

    app->m_window.pollEvents();

    double currentTime = emscripten_get_now(); 
    
    double deltaTime = (currentTime - app->m_lastTime) / 1000.0f;
    app->m_lastTime = currentTime;
    app->m_worldTime = (float)currentTime / 1000.0f;

    //if (deltaTime > 0.1) deltaTime = 0.1;

    app->update((float)deltaTime);
    app->render();
}

EM_BOOL Application::onMouseCallback(int eventType, const EmscriptenMouseEvent *e, void *userData) 
{
    Application* app = static_cast<Application*>(userData);
    return app->handleOnMouseCallback(eventType, e);
}

EM_BOOL Application::onWheelCallback(int eventType, const EmscriptenWheelEvent *e, void *userData) 
{
    Application* app = static_cast<Application*>(userData);
    return app->handleOnWheelCallback(eventType, e);
}

EM_BOOL Application::onTouchCallback(int eventType, const EmscriptenTouchEvent *e, void *userData) 
{
    Application* app = static_cast<Application*>(userData);
    return app->handleOnTouchCallback(eventType, e);
}

EM_BOOL Application::handleOnMouseCallback(int eventType, const EmscriptenMouseEvent *e) 
{
    ImGuiIO& io = ImGui::GetIO();
    float pixelRatio = (float)emscripten_get_device_pixel_ratio();
    
    io.MousePos = ImVec2((float)e->targetX, (float)e->targetY);
    
    if (eventType == EMSCRIPTEN_EVENT_MOUSEDOWN) {
        if (e->button == 0) io.MouseDown[0] = true;
        if (e->button == 2) io.MouseDown[1] = true;
    }
    if (eventType == EMSCRIPTEN_EVENT_MOUSEUP) {
        if (e->button == 0) io.MouseDown[0] = false;
        if (e->button == 2) io.MouseDown[1] = false;
    }
    return io.WantCaptureMouse;
}

EM_BOOL Application::handleOnWheelCallback(int eventType, const EmscriptenWheelEvent *e) 
{
    ImGuiIO& io = ImGui::GetIO();
    io.MouseWheel += (float)-e->deltaY * 0.01f;
    return io.WantCaptureMouse;
}

EM_BOOL Application::handleOnTouchCallback(int eventType, const EmscriptenTouchEvent *e) 
{
    ImGuiIO& io = ImGui::GetIO();

    if (e->numTouches > 0) {
        io.MousePos = ImVec2((float)e->touches[0].targetX, (float)e->touches[0].targetY);
        
        if (eventType == EMSCRIPTEN_EVENT_TOUCHSTART) io.MouseDown[0] = true;
        if (eventType == EMSCRIPTEN_EVENT_TOUCHEND || eventType == EMSCRIPTEN_EVENT_TOUCHCANCEL) 
            io.MouseDown[0] = false;
    }
    return io.WantCaptureMouse;
}