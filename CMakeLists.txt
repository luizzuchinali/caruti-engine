cmake_minimum_required(VERSION 3.26)
project(caruti_engine)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS
        "src/**.cpp"
        "third/glad/**.c"
        "third/stb/**.cpp"
)

#ImGui
set(IMGUI_DIR third/imgui)

add_executable(caruti_engine ${sources}
        ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp
        ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
        ${IMGUI_DIR}/misc/cpp/imgui_stdlib.cpp
        ${IMGUI_DIR}/imgui.cpp
        ${IMGUI_DIR}/imgui_draw.cpp
        ${IMGUI_DIR}/imgui_demo.cpp
        ${IMGUI_DIR}/imgui_tables.cpp
        ${IMGUI_DIR}/imgui_widgets.cpp
)

#Copy resources
add_custom_target(copy_resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/resources
        ${CMAKE_CURRENT_BINARY_DIR}/resources)
add_dependencies(caruti_engine copy_resources)

# Include
include_directories(caruti_engine src)
target_include_directories(caruti_engine PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/third/glfw/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third/glad/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third/stb/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third/glm/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third/imgui
)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/third/fmt EXCLUDE_FROM_ALL)

# Link

IF (WIN32)
    target_link_libraries(caruti_engine PRIVATE
            ${CMAKE_CURRENT_SOURCE_DIR}/third/glfw/lib/win/libglfw3.a
            fmt::fmt
    )
ELSE ()

ENDIF ()