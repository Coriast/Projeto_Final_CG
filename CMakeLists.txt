# Quando der erro de Unresolved External Symbol de Delete e Generate no Cache

# quando for instalar um pacote por vcpkg tem que colocar :x64-windows
# Ao instalar o vcpkg lembrar de fazer o integrate install para o Visual Studio

cmake_minimum_required (VERSION 3.8)

project ("OpenGL_Unwrapped")

include_directories(include "include/glad" "include/KHR" "include/CGHelpers")

file(GLOB SOURCES "src/*.cpp" "src/*.c" "include/*.h" "include/*.hpp")


# Add source to this project's executable.
add_executable (OpenGL_Unwrapped ${SOURCES})

find_package(glfw3 REQUIRED)

#find_package(assimp REQUIRED)   

find_package(glm REQUIRED)

find_package(OpenGL REQUIRED)

target_link_libraries(OpenGL_Unwrapped PRIVATE glfw glm::glm ${OPENGL_LIBRARY}) #assimp::assimp

# TODO: Add tests and install targets if needed.
