cmake_minimum_required(VERSION 3.10)
project(EmscriptenWebGL2)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(IMGUI_SOURCES
    imgui/imgui.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/backends/imgui_impl_opengl3.cpp
)

add_executable(my_web_app main.cpp ${IMGUI_SOURCES})

set_target_properties(my_web_app PROPERTIES 
    OUTPUT_NAME "index"
    SUFFIX ".html"
)

target_include_directories(my_web_app PRIVATE 
    /usr/local/include
    ${CMAKE_SOURCE_DIR}/imgui
    ${CMAKE_SOURCE_DIR}/imgui/backends
)

target_link_options(my_web_app PRIVATE
    #"-sFORCE_FILESYSTEM=1"
    "-sUSE_WEBGL2=1"
    "-sFULL_ES3=1"
    "-sWASM=1"
    "-O3"
    "--shell-file=${CMAKE_SOURCE_DIR}/shell_minimal.html"
    #"--preload-file=${CMAKE_SOURCE_DIR}/texture.png@texture.png"
    "--preload-file=${CMAKE_SOURCE_DIR}/shader.vert@shader.vert"
    "--preload-file=${CMAKE_SOURCE_DIR}/shader.frag@shader.frag"
    "--preload-file=${CMAKE_SOURCE_DIR}/screen_shader.vert@screen_shader.vert"
    "--preload-file=${CMAKE_SOURCE_DIR}/screen_shader.frag@screen_shader.frag"
    #"--preload-file=${CMAKE_SOURCE_DIR}/model.geom@model.geom"
    # "--preload-file=${CMAKE_SOURCE_DIR}/tree.geom@tree.geom"
    # "--preload-file=${CMAKE_SOURCE_DIR}/tree.png@tree.png"
    #"--preload-file=${CMAKE_SOURCE_DIR}/tree.tga@tree.tga"
    "--preload-file=${CMAKE_SOURCE_DIR}/ogre.png@ogre.png"
    "--preload-file=${CMAKE_SOURCE_DIR}/ogre.geom@ogre.geom"
)

target_compile_definitions(my_web_app PRIVATE EMSCRIPTEN)