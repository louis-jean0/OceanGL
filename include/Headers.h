#pragma once

// Glad & GLFW
#include <glad/glad.h>
#include <GLFW/glfw3.h>

// Imgui
#include <Imgui/imgui.h>
#include <Imgui/imgui_impl_glfw.h>
#include <Imgui/imgui_impl_opengl3.h>

// GLM
#include <../external/glm/ext.hpp>
#include <../external/glm/vec2.hpp> 
#include <../external/glm/vec3.hpp> 
#include <../external/glm/vec4.hpp> 
#include <../external/glm/mat4x4.hpp> 
#include <../external/glm/ext/matrix_transform.hpp> 
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