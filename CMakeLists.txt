cmake_minimum_required(VERSION 3.26)
project(caruti_engine)

set(CMAKE_CXX_STANDARD 23)

file(GLOB_RECURSE sources CONFIGURE_DEPENDS "src/**.cpp" "src/**.hpp" "third/glad/**.c")
add_executable(caruti_engine ${sources})

#Copy resources
add_custom_target(copy_resources
        COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_CURRENT_SOURCE_DIR}/Resources
        ${CMAKE_CURRENT_BINARY_DIR}/Resources)
add_dependencies(caruti_engine copy_resources)

# Include
include_directories(caruti_engine src)
target_include_directories(caruti_engine PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/third/glfw/include
        ${CMAKE_CURRENT_SOURCE_DIR}/third/glad/include
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

