cmake_minimum_required(VERSION 3.15)

project(PacMan)

find_package(OpenGL REQUIRED)

add_subdirectory(glad)
add_subdirectory(glfw)
add_subdirectory(glm)

add_executable(PacMan  "main.cpp" "shaders/shader.h" "pacman.h" "pacman.cpp" "move.cpp" "ghost.h" "ghost.cpp")
target_link_libraries(PacMan glfw glad OpenGL::GL ${CMAKE_DL_LIBS})
