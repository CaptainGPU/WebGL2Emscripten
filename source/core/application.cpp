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
    screenShaderProgram = createShaderProgram("screen_shader.vert", "screen_shader.frag");
    initScreenQuads();
    //initTexture("ogre.png");

    m_mainShader = std::make_unique<Shader>("shader.vert", "shader.frag");
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
    
    m_ogreTransform.scale = {1.0f, 1.0f, 1.0f};
    m_ogreTransform.position = {0.0f, 0.0f, 0.0f};
}

void Application::update(float dt)
{
    float currentFPS = (dt > 0) ? 1.0f / dt : 0.0f;

    m_fpsHistory[m_fpsOffset] = currentFPS;
    m_fpsOffset = (m_fpsOffset + 1) % FPS_HISTORY_SIZE;

    m_ogreTransform.rotation.y += m_rotationSpeed * dt;
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
        initFBO(canvasW, canvasH);
    }


    // FIRST PASS

    glEnable(GL_DEPTH_TEST);

    glBindFramebuffer(GL_FRAMEBUFFER, fbo);

    glViewport(0, 0, canvasW, canvasH);

    glm::vec3 color = glm::vec3(0.25f, 0.5f, 1.0f) * 0.25f;

    glClearColor(color.r, color.g, color.b, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    float aspect = m_window.getAspectRatio();
    glm::mat4 projection = m_camera.getProjectionMatrix(aspect);
    glm::mat4 view = m_camera.getViewMatrix();

    glm::mat4 model = m_ogreTransform.getModelMatrix();

    m_ogre->draw(view, projection);

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
    
    ImGui::ColorEdit3("Light", glm::value_ptr(m_lightColor), ImGuiColorEditFlags_Float);
    
    ImGui::Checkbox("Use Texture", &m_useTexture);

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

GLuint Application::createShaderProgram(const std::string& vertPath, const std::string& fragPath)
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

std::string Application::readFile(const std::string& filePath)
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

void Application::initScreenQuads()
{
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

void Application::initTexture(const char* texturePath)
{
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);


    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

    int width, height, nrChannels;

    stbi_set_flip_vertically_on_load(true);
    unsigned char *data = stbi_load(texturePath, &width, &height, &nrChannels, 0);

    if (data) 
    {
        GLenum format = (nrChannels == 4) ? GL_RGBA : GL_RGB;
        glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
        printf("Texture loaded successfully: %dx%d\n", width, height);
    } else {
        printf("Failed to load texture. Reason: %s\n", stbi_failure_reason());
    }
}

void Application::initFBO(int width, int height)
{
    if (fbo != 0) {
        glDeleteFramebuffers(1, &fbo);
        glDeleteTextures(1, &textureColorBuffer);
        glDeleteRenderbuffers(1, &rbo);
    }
    printf("initFBO\n");

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

    m_currentFBOWidth = width;
    m_currentFBOHeight = height;
    printf("FBO initialized/resized to: %dx%d\n", width, height);
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