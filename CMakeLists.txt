# CMakeList.txt : CMake project for TheWitness_CG, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

project ("TheWitness_CG")

include_directories(include "include/glad" "include/KHR")

file(GLOB SOURCES "src/*.cpp" "src/*.c" "include/*.h" "include/*.hpp")


# Add source to this project's executable.
add_executable (TheWitness_CG ${SOURCES})

find_package(glfw3 REQUIRED)

find_package(glm REQUIRED)

find_package(OpenGL REQUIRED)

target_link_libraries(TheWitness_CG PRIVATE glfw glm::glm ${OPENGL_LIBRARY})

# TODO: Add tests and install targets if needed.
