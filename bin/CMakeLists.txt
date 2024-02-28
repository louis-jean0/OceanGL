cmake_minimum_required(VERSION 3.0)

project(main)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

include_directories(../include)
link_directories(../lib)

file(GLOB SOURCES ../src/*.cpp ../src/Imgui/*.cpp ../src/glad.c)

add_executable(main ${SOURCES})

# Link GLFW library
target_link_libraries(main glfw3)