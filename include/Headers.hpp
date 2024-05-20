#pragma once

// Glad & GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Imgui
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

// GLM
#include <../external/glm/ext.hpp>
#include <../external/glm/vec2.hpp> 
#include <../external/glm/vec3.hpp> 
#include <../external/glm/vec4.hpp> 
#include <../external/glm/mat4x4.hpp> 
#include <../external/glm/ext/matrix_clip_space.hpp>
#include <../external/glm/gtc/matrix_transform.hpp> 
#include <../external/glm/gtc/type_ptr.hpp>


// Common
#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <cmath>
#include <stdbool.h>
#include <unistd.h>
#include <sys/sysinfo.h>
#include <thread>

#include <Window.hpp>
#include <Plane.hpp>
#include <Orbital.hpp>
#include <Skybox.hpp>
#include <EBO.hpp>
#include <VBO.hpp>
#include <VAO.hpp>
#include <Shader.hpp>
#include <ComputeShader.hpp>
#include <Sound.hpp>
#include <Flotability_2.hpp>


#include <stb_image.hpp>
#include <Texture.hpp>
