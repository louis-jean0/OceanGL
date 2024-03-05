cmake_minimum_required(VERSION 3.10)

# Set the project name and version
project(OceanGL VERSION 0.1)

# Specify the C++ standard
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Add the GLFW subdirectory if you have cloned GLFW into your project
# This assumes that GLFW's CMakeLists.txt is in the external/glfw directory
set(GLFW_BUILD_DOCS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
add_subdirectory(external/glfw-3.3.9)

# Glad does not use CMake, you have to add it manually
add_library(glad STATIC external/glad/src/glad.c)
target_include_directories(glad PRIVATE external/glad/include)

# Add the executable
add_executable(OceanGL src/OceanGL.cpp src/Window.cpp src/Plane.cpp src/Camera.cpp src/shader.cpp)

# Specify the directories where to find the header files
include_directories(external/glfw-3.3.9/include)
include_directories(external/glad/include)
include_directories(external/glm-1.0.1/include)

# Link the libraries to the executable
target_link_libraries(OceanGL glfw glad)

# Définir le chemin d'ImGui
set(IMGUI_DIR ${CMAKE_CURRENT_SOURCE_DIR}/external/imgui-1.90.4)

# Ajouter la bibliothèque ImGui
file(GLOB IMGUI_SOURCES ${IMGUI_DIR}/src/*.cpp)
add_library(imgui STATIC ${IMGUI_SOURCES})
target_include_directories(imgui PUBLIC ${IMGUI_DIR}/include)
target_include_directories(imgui PUBLIC ${IMGUI_DIR}/backends)

# Lier OpenGL et GLFW à ImGui
find_package(OpenGL REQUIRED)
target_link_libraries(imgui PUBLIC ${OPENGL_gl_LIBRARY})
target_link_libraries(imgui PUBLIC glfw)

# Ajouter les implémentations spécifiques à la bibliothèque de votre système de fenêtrage et de rendu
target_sources(imgui PUBLIC 
    ${IMGUI_DIR}/backends/imgui_impl_glfw.cpp 
    ${IMGUI_DIR}/backends/imgui_impl_opengl3.cpp
)

# Ajoutez les répertoires d'en-tête nécessaires pour ces implémentations
target_include_directories(imgui PUBLIC 
    ${IMGUI_DIR}/backends
    # ${IMGUI_DIR}/misc/freetype # Si vous utilisez FreeType, assurez-vous d'ajouter les dépendances requises
)

# Assurez-vous que ImGui trouve également les en-têtes de GLFW et GLAD
target_include_directories(imgui PUBLIC external/glfw-3.3.9/include)
target_include_directories(imgui PUBLIC external/glad/include)

# Link ImGui with the OceanGL executable
target_link_libraries(OceanGL imgui)

target_include_directories(OceanGL PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

set_target_properties(glad imgui PROPERTIES
    ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
    LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
)
