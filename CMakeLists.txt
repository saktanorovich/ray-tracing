
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

find_package(OpenGL REQUIRED)
find_package(glfw3 REQUIRED)
find_package(glm CONFIG REQUIRED)

target_link_libraries(rt PRIVATE ${OPENGL_LIBRARIES} glfw glm::glm)

if(APPLE)
    target_link_libraries(rt PRIVATE "-framework Cocoa" "-framework OpenGL" "-framework IOKit")
elseif(WIN32)
    target_link_libraries(rt PRIVATE opengl32)
endif()

set_target_properties(rt PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin)
