cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(OceanGL VERSION 0.1)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# If you want to use the installed GLFW on your system, uncomment this and comment out the add_subdirectory part
# find_package(GLFW3 REQUIRED)

# Add the GLFW subdirectory if you have cloned GLFW into your project
# This assumes that GLFW's CMakeLists.txt is in the external/glfw directory
add_subdirectory(external/glfw-3.3.9)

# Glad does not use CMake, you have to add it manually
add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PRIVATE external/glad/include)

# Add the executable
add_executable(OceanGL src/OceanGL.cpp)

# Link the libraries to the executable
# If you used find_package(GLFW3) above, you should link to glfw instead of glfw3
target_link_libraries(OceanGL glfw glad)

# Specify the directories where to find the header files
target_include_directories(OceanGL PRIVATE external/glfw-3.3.9/include)
target_include_directories(OceanGL PRIVATE external/glad/include)

# If GLFW and GLAD require additional libraries on your system, link them here as well