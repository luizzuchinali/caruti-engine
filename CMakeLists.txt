cmake_minimum_required(VERSION 3.26)
project(caruti_engine)

set(CMAKE_CXX_STANDARD 23)

set(GLFW_DIR third/glfw)
set(GLM_DIR third/glm)
set(FMT_DIR third/fmt)
set(GLAD_DIR third/glad)
set(STB_DIR third/stb)
set(IMGUI_DIR third/imgui)
set(ASSIMP_DIR third/assimp)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS
        "src/**.cpp"
        "src/**.hpp"
        ${GLAD_DIR}/**.c
        ${STB_DIR}/**.cpp
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
)

add_executable(caruti_engine ${sources})

#Copy resources
add_custom_target(copy_resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        ${CMAKE_CURRENT_BINARY_DIR}/resources)
add_dependencies(caruti_engine copy_resources)

# Includes
include_directories(caruti_engine src)
target_include_directories(caruti_engine PRIVATE
        ${GLFW_DIR}/include
        ${GLAD_DIR}/include
        ${STB_DIR}/include
        ${GLM_DIR}/include
        ${IMGUI_DIR}
        ${ASSIMP_DIR}/include
)

#GLFW
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(${GLFW_DIR})

#ASSIMP
set(BUILD_SHARED_LIBS OFF CACHE BOOL "" FORCE)
set(ASSIMP_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(ASSIMP_INJECT_DEBUG_POSTFIX OFF CACHE BOOL "" FORCE)
set(ASSIMP_INSTALL OFF CACHE BOOL "" FORCE)
add_subdirectory(${ASSIMP_DIR})

#FMT
add_subdirectory(${FMT_DIR})

target_link_libraries(caruti_engine PRIVATE
        glfw
        assimp
        fmt
)