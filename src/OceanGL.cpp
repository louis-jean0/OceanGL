#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <imgui.h>
#include <imgui_impl_glfw.h>
#include <imgui_impl_opengl3.h>

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

#include "Window.h"
#include "Plane.h"
#include "Camera.h"
#include "shader.hpp"

long long GetMemoryUsage() {
    long long memory_usage = 0;
    std::ifstream status_file("/proc/self/status");
    if (status_file.is_open()) {
        std::string line;
        while (std::getline(status_file, line)) {
            if (line.find("VmRSS:") != std::string::npos) {
                std::istringstream iss(line);
                std::string label;
                long long value;
                iss >> label >> value;
                memory_usage = value;
                break;
            }
        }
        status_file.close();
    }
    return memory_usage;
}

void processInput(GLFWwindow *window, Camera camera)
{
    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS){
        glfwSetWindowShouldClose(window, true);
    }
}

int main() {

    Plane plane = Plane(1.5, 1.5, glm::vec3(-1.0,-1.0,0.0));

    Window window(4, 6, 1280, 720, "TER - OceanGL", true);
    window.setup_GLFW();

    Camera camera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, 0.0f, 0.0f), glm::vec3(0.0f, 1.0f,  0.0f));

    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.get_window(), true);
    ImGui_ImplOpenGL3_Init("#version 460");

    glClearColor(225.0f, 235.0f, 228.0f, 1.0f);

    glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    GLuint programID = LoadShaders( "../shaders/vertex_shader.glsl", "../shaders/fragment_shader.glsl" );

     // Enable depth test
    glEnable(GL_DEPTH_TEST);
    // Accept fragment if it closer to the camera than the former one
    glDepthFunc(GL_LESS);

    GLuint VertexArrayID;
    glGenVertexArrays(1, &VertexArrayID);
    glBindVertexArray(VertexArrayID);

    // Generate buffers
    GLuint vertexbuffer;
    glGenBuffers(1, &vertexbuffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
    glBufferData(GL_ARRAY_BUFFER, plane.getIndexedVertices().size() * sizeof(glm::vec3), &(plane.getIndexedVertices()[0]), GL_STATIC_DRAW);

    GLuint elementbuffer;
    glGenBuffers(1, &elementbuffer);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, plane.getIndices().size() * sizeof(unsigned short), &(plane.getIndices()[0]) , GL_STATIC_DRAW);

    GLuint ModelMatrix = glGetUniformLocation(programID,"Model");
    GLuint ViewMatrix = glGetUniformLocation(programID,"View");
    GLuint ProjectionMatrix = glGetUniformLocation(programID,"Projection");

    float actualRotation = 0.0f;

    while (!glfwWindowShouldClose(window.get_window())) {

        processInput(window.get_window(), camera);

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        glUseProgram(programID);

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        float fps = ImGui::GetIO().Framerate;
        float ms_per_frame = 1000.0f / fps;

        ImGui::Begin("Paramètres");

        ImGui::Separator();
        ImGui::Spacing();

        ImGui::Text("Métriques de l'ordinateur (Debug) :");
        ImGui::Spacing();
        ImGui::Spacing();
        ImGui::Spacing();

        char hostname[256];
        gethostname(hostname, sizeof(hostname));
        ImGui::Text("Nom de l'ordinateur: %s", hostname);

        FILE *fp = popen("uname -sr", "r");
        char os[256];
        fgets(os, sizeof(os), fp);
        pclose(fp);
        ImGui::Text("Système d'exploitation: %s", os);

        struct sysinfo si;
        sysinfo(&si);
        double ram_totale = si.totalram * si.mem_unit / (1024 * 1024 * 1024); // Convertir en Go
        ImGui::Text("Mémoire RAM totale : %.2f Go", ram_totale);

        long long ram_utilisee_programme = GetMemoryUsage();
        double ram_utilisee_go = static_cast<double>(ram_utilisee_programme) / (1024 * 1024); // Convertir en Mo
        double pourcentage_ram_utilisee = (ram_utilisee_go / ram_totale) * 100.0; // Calculer le pourcentage
        ImGui::Text("Mémoire RAM utilisée par le programme : %.4f Mo", ram_utilisee_go);
        ImGui::Text("Mémoire RAM utilisée par le programme (en %%) : %.4f%%", pourcentage_ram_utilisee);
        ImGui::Text("FPS : %.2f / MS (Milliseconds Per Frame) : %.2f", fps, ms_per_frame);

        ImGui::Spacing();
        ImGui::Separator();

        ImGui::Text("Autres");

        ImGui::End();

        /*
        int display_w, display_h;
        glfwGetFramebufferSize(window.get_window(), &display_w, &display_h);
        glViewport(0, 0, 1280, 720);
        */
        
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); 

        // MVP matrix
        glm::mat4 Model = glm::mat4(1.f);

        camera.setTarget(glm::normalize(plane.getCenter() - camera.getPosition()));
        glm::mat4 View = glm::lookAt(camera.getPosition(), camera.getPosition() + camera.getTarget(), camera.getUp());

        View = glm::rotate(View, glm::radians(actualRotation),glm::vec3(0.0f,1.0f,0.0f)); // Pour faire une rotation constante
        if (actualRotation > 360){
            actualRotation = 0.0;
        }
        actualRotation += 1.0;

        glm::mat4 Projection = glm::perspective(glm::radians(45.0f), (float)1280/(float)720,0.1f,100.0f);


        // Send our transformation to the currently bound shader,
        // in the "Model View Projection" to the shader uniforms
        glUniformMatrix4fv(ModelMatrix,1,GL_FALSE,&Model[0][0]);
        glUniformMatrix4fv(ViewMatrix,1,GL_FALSE,&View[0][0]);
        glUniformMatrix4fv(ProjectionMatrix,1,GL_FALSE,&Projection[0][0]);


        // Draw the plane
        glEnableVertexAttribArray(0);
        glBindBuffer(GL_ARRAY_BUFFER, vertexbuffer);
        glVertexAttribPointer(
                    0,                  // attribute
                    3,                  // size
                    GL_FLOAT,           // type
                    GL_FALSE,           // normalized?
                    0,                  // stride
                    (void*)0            // array buffer offset
                    );

        // Index buffer
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, elementbuffer);

        // Draw the triangles !
        glDrawElements(
                    GL_TRIANGLES,      // mode
                    plane.getIndices().size(),    // count
                    GL_UNSIGNED_SHORT,   // type
                    (void*)0           // element array buffer offset
                    );

        glDisableVertexAttribArray(0);

        glfwSwapBuffers(window.get_window()); 
        glfwPollEvents();   
    }

    glDeleteBuffers(1, &vertexbuffer);
    glDeleteBuffers(1, &elementbuffer);
    glDeleteProgram(programID);
    glDeleteVertexArrays(1, &VertexArrayID);

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();


    window.~Window();
    
    return 0;
}