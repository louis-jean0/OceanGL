// En-têtes de base
#include <Headers.h>

// En-têtes de l'application
#include <Window.h>


int main() {
    Window window(4, 6, 1280, 720, "TER - OceanGL", true);
    window.setup_GLFW();


    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.get_window(), true);
    ImGui_ImplOpenGL3_Init("#version 460");


    while(!glfwWindowShouldClose(window.get_window()))
    {
        glfwPollEvents();   

        // Début du rendu ImGui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        // Création de l'interface utilisateur
        ImGui::Begin("Hello, world!");
        ImGui::Text("This is some useful text.");
        ImGui::End();

        // Fin du rendu ImGui
        ImGui::Render();

        int display_w, display_h;
        glfwGetFramebufferSize(window.get_window(), &display_w, &display_h);
        glViewport(0, 0, 1280, 720);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        glfwSwapBuffers(window.get_window()); 
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    window.~Window();
    
    return 0;
}