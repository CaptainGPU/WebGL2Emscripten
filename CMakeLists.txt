cmake_minimum_required(VERSION 3.10)
project(EmscriptenWebGL2)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_DIR "${CMAKE_SOURCE_DIR}/source")
set(EXTERNAL_DIR "${CMAKE_SOURCE_DIR}/thirdparty")

set(IMGUI_SOURCES
    "${EXTERNAL_DIR}/imgui/imgui.cpp"
    "${EXTERNAL_DIR}/imgui/imgui_draw.cpp"
    "${EXTERNAL_DIR}/imgui/imgui_tables.cpp"
    "${EXTERNAL_DIR}/imgui/imgui_widgets.cpp"
    "${EXTERNAL_DIR}/imgui/backends/imgui_impl_opengl3.cpp"
)

add_executable(my_web_app)

target_sources(my_web_app PRIVATE 
    "${SOURCE_DIR}/main.cpp"

    "${SOURCE_DIR}/core/window.cpp"
    "${SOURCE_DIR}/core/application.cpp"

    "${SOURCE_DIR}/renderer/camera.h"
    "${SOURCE_DIR}/renderer/transform.h"

    ${IMGUI_SOURCES}
)

target_include_directories(my_web_app PRIVATE 
    "${SOURCE_DIR}"
    "${CMAKE_SOURCE_DIR}"
    "${EXTERNAL_DIR}/imgui"
    "${EXTERNAL_DIR}/imgui/backends"
    "${EXTERNAL_DIR}"
)

set_target_properties(my_web_app PROPERTIES 
    OUTPUT_NAME "index"
    SUFFIX ".html"
)

target_link_options(my_web_app PRIVATE
    #"-sFORCE_FILESYSTEM=1"
    "-sUSE_WEBGL2=1"
    "-sFULL_ES3=1"
    "-sWASM=1"
    "-O3"
    # "-sASSERTIONS=1"
    # "-fsanitize=address"
    "--shell-file=${CMAKE_SOURCE_DIR}/assets/web/shell_minimal.html"

    "--preload-file=${CMAKE_SOURCE_DIR}/shaders/shader.vert@shader.vert"
    "--preload-file=${CMAKE_SOURCE_DIR}/shaders/shader.frag@shader.frag"
    "--preload-file=${CMAKE_SOURCE_DIR}/shaders/screen_shader.vert@screen_shader.vert"
    "--preload-file=${CMAKE_SOURCE_DIR}/shaders/screen_shader.frag@screen_shader.frag"

    "--preload-file=${CMAKE_SOURCE_DIR}/assets/textures/ogre.png@ogre.png"
    # "--preload-file=${CMAKE_SOURCE_DIR}/assets/textures/texture.png@texture.png"
    # "--preload-file=${CMAKE_SOURCE_DIR}/assets/textures/tree.png@tree.png"
    # "--preload-file=${CMAKE_SOURCE_DIR}/assets/textures/tree.tga@tree.tga"
    
    "--preload-file=${CMAKE_SOURCE_DIR}/assets/models/ogre.geom@ogre.geom"
    # "--preload-file=${CMAKE_SOURCE_DIR}/assets/models/model.geom@model.geom"
    # "--preload-file=${CMAKE_SOURCE_DIR}/assets/models/tree.geom@tree.geom"
)

target_compile_definitions(my_web_app PRIVATE EMSCRIPTEN)