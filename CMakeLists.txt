cmake_minimum_required(VERSION 3.15)
project(rt)

set(CMAKE_CXX_STANDARD 17)

set(SOURCES
    src/glad.cpp
    src/camera3d.cpp
    src/scene3d.cpp
    src/main.cpp
    src/math.cpp
)

add_executable(rt ${SOURCES})
target_include_directories(rt PRIVATE ${CMAKE_SOURCE_DIR}/include)
target_compile_definitions(rt PRIVATE RT_SHADER_DIR="${CMAKE_SOURCE_DIR}/src/shaders")

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)

target_link_libraries(rt PRIVATE ${OPENGL_LIBRARIES} glfw glm::glm)

if (APPLE)
    target_link_libraries(rt PRIVATE "-framework Cocoa" "-framework OpenGL" "-framework IOKit")
elseif (WIN32)
    target_link_libraries(rt PRIVATE opengl32)
endif()

set_target_properties(rt PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)

# Keep a copy of the shaders next to the executable so it runs from any directory.
# Driven by a stamp file rather than POST_BUILD, so editing a shader alone (without
# touching any .cpp) still refreshes bin/shaders on the next build.
# Note: adding a *new* shader file requires re-running cmake to update this glob.
file(GLOB RT_SHADER_FILES CONFIGURE_DEPENDS
    ${CMAKE_SOURCE_DIR}/src/shaders/*.vert
    ${CMAKE_SOURCE_DIR}/src/shaders/*.frag
)

add_custom_command(
    OUTPUT ${CMAKE_CURRENT_BINARY_DIR}/shaders.stamp
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/src/shaders
        ${CMAKE_SOURCE_DIR}/bin/shaders
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_BINARY_DIR}/shaders.stamp
    DEPENDS ${RT_SHADER_FILES}
    COMMENT "Copying shaders to bin/shaders"
    VERBATIM
)

add_custom_target(rt_shaders DEPENDS ${CMAKE_CURRENT_BINARY_DIR}/shaders.stamp)
add_dependencies(rt rt_shaders)
