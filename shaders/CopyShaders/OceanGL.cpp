#include <Headers.hpp>

void processInput(GLFWwindow *window);
void initGUI(GLFWwindow* window);
void renderGUI();
void destroyGUI();

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

// Window settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// Timing
float deltaTime = 0.0f;	
float lastFrame = 0.0f;

// Orbital camera settings
float horizontal_angle = 3.14f; 
float vertical_angle = 0.0f; 
float radius = 20.0f;

// Camera settings
glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, -3.5f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f,  0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);


// Light settings (here for ImGui)
float ambientStrength = 0.1f;
float diffuseStrength = 1.0f;
float specularStrength = 0.5f;
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

//Sin settings (for plane)
float amplitude = 1.;   // amplitude
float L = 1.0;  // distance pic2crête
float S = 1.0;  // Speed


/*
float amplitude = 1.;
float x = 1.;
float k = 1.;
float af = 1.;
float p = 0.;*/

// Gerstner 
float L = 1.0;
float steepness = 1.0;

// Booléens modèles
bool ModeleSin = true;
bool ModeleGerstner = false;

// Booléen wireframe
bool fildefer = true;

// Plan
int resolution = 512;
int gridSize = 1;

int main() {
    
    // Window
    Window window(4, 6, SCR_WIDTH, SCR_HEIGHT, "TER - OceanGL", true);
    window.setup_GLFW();

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.get_window(), true);
    ImGui_ImplOpenGL3_Init("#version 460");

    // Plane
    std::vector<Plane> grid;
    for(int i = 0; i < gridSize; i++) {
        for(int j = 0; j < gridSize; j++) {
            Plane plane(1, 512);
            plane.attachShader("../shaders/SinWave.vert", "../shaders/SinWave.frag");
            plane.createPlane();
            grid.push_back(plane);
        }       
    }

    glEnable(GL_DEPTH_TEST);

    // Render loop
    while (!glfwWindowShouldClose(window.get_window())) {
        // Variables de la boucle
        float fps = ImGui::GetIO().Framerate;
        float ms_per_frame = 1000.0f / fps;
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;

        // Camera
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::lookAt(cameraPos,cameraTarget,cameraUp);
        glm::mat4 projection = glm::perspective(45.0f,(float)SCR_WIDTH/(float)SCR_HEIGHT,0.1f,500.0f);

        // Gestion des entrées
        processInput(window.get_window());

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Imgui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

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

        ImGui::Text("Paramètres de la lumière :");
        ImGui::Spacing();

        ImGui::SliderFloat3("Position de la lumière", glm::value_ptr(lightPos), -10.0f, 10.0f);
        ImGui::SliderFloat("Ambiant", &ambientStrength, 0.0f, 1.0f);
        ImGui::SliderFloat("Diffuse", &diffuseStrength, 0.0f, 2.0f);
        ImGui::SliderFloat("Spéculaire", &specularStrength, 0.0f, 1.0f);
        ImGui::Separator();

        ImGui::Spacing();

        if (ImGui::Checkbox("Modèle sinusoïdal", &ModeleSin)) {
            ModeleGerstner = false;

            for(int i = 0; i < gridSize * gridSize; i++) {
                grid[i].detachShader();
                grid[i].attachShader("../shaders/SinWave.vert", "../shaders/SinWave.frag");
                grid[i].createPlane();   
            }
        }
        
        ImGui::SameLine();

        if (ImGui::Checkbox("Modèle de Gerstner", &ModeleGerstner)) {
            ModeleSin = false;

            for(int i = 0; i < gridSize * gridSize; i++) {
                grid[i].detachShader();
                grid[i].attachShader("../shaders/GerstnerWave.vert", "../shaders/SinWave.frag");
                grid[i].createPlane();
            }     
        }

        ImGui::Spacing();

        if (ModeleSin) {
            for(int i = 0; i < gridSize * gridSize; i++) {
                glm::mat4 model_with_translation = glm::translate(model, glm::vec3((i % gridSize) * 1.0f, 0.0f, (i / gridSize) * 1.0f));                                    grid[i].getShader().setBindMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model_with_translation));
                grid[i].getShader().setBindMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model_with_translation));
                grid[i].getShader().setBindMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
                grid[i].getShader().setBindMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
                grid[i].getShader().setBind1f("time", glfwGetTime());
                grid[i].getShader().setBind1f("amplitude", amplitude);
                grid[i].getShader().setBind1f("x", x);
                grid[i].getShader().setBind1f("k", k);
                grid[i].getShader().setBind1f("af", af);
                grid[i].getShader().setBind1f("p", p); 

                grid[i].useShader();
                grid[i].updatePlane(GL_TRIANGLES);     
            }

            ImGui::Text("Paramètres du shader sinusoïdal :");
            ImGui::SliderFloat("Amplitude", &amplitude, 0.0f, 20.0f);
            ImGui::SliderFloat("X", &x, 0.0f, 20.0f);
            ImGui::SliderFloat("K", &k, 0.0f, 20.0f);
            ImGui::SliderFloat("AF", &af, 0.0f, 20.0f);
            ImGui::SliderFloat("P", &p, 0.0f, 20.0f);
            ImGui::Separator();
        }

        if (ModeleGerstner) {
            for(int i = 0; i < gridSize * gridSize; i++) {
                glm::mat4 model_with_translation = glm::translate(model, glm::vec3((i % gridSize) * 2.0f, 0.0f, (i / gridSize) * 2.0f));                                    
                grid[i].getShader().setBindMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model_with_translation));
                grid[i].getShader().setBindMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));                  
                grid[i].getShader().setBindMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
                grid[i].getShader().setBind1f("time",glfwGetTime());
                grid[i].getShader().setBind1f("amplitude", amplitude);
                grid[i].getShader().setBind1f("PI", M_PI);
                grid[i].getShader().setBind1f("g", 9.81); // constante gravitationelle
                grid[i].getShader().setBind1f("L", L);
                grid[i].getShader().setBind1f("steepness", steepness);

                grid[i].useShader();
                grid[i].updatePlane(GL_TRIANGLES);

        if (i % gridSize != gridSize - 1) { // Vérifier si le plan a un voisin à droite
            int right_index = i + 1;
            glm::vec3 right_translation = glm::vec3((right_index % gridSize) * 2.0f, 0.0f, (right_index / gridSize) * 2.0f);
            glm::mat4 right_model_with_translation = glm::translate(model, right_translation);
            grid[right_index].getShader().setBindMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(right_model_with_translation));
            grid[right_index].useShader();
            grid[right_index].updatePlane(GL_TRIANGLES);
        }

        if (i < gridSize * (gridSize - 1)) { // Vérifier si le plan a un voisin en bas
            int bottom_index = i + gridSize;
            glm::vec3 bottom_translation = glm::vec3((bottom_index % gridSize) * 2.0f, 0.0f, (bottom_index / gridSize) * 2.0f);
            glm::mat4 bottom_model_with_translation = glm::translate(model, bottom_translation);
            grid[bottom_index].getShader().setBindMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(bottom_model_with_translation));
            grid[bottom_index].useShader();
            grid[bottom_index].updatePlane(GL_TRIANGLES);
        }
            }       

            ImGui::Text("Paramètres du shader Gerstner :");
            ImGui::SliderFloat("Amplitude", &amplitude, 0.0f, 20.0f);
            ImGui::SliderFloat("L", &L, 0.0f, 10.0f);
            ImGui::SliderFloat("Steepness", &steepness, 0.0f, 10.0f);
        }

        ImGui::Separator();

        ImGui::Spacing();

        ImGui::Text("Paramètres du plan :");
        if(ImGui::SliderInt("Résolution", &resolution, 0.0f, 2048.0f)) {
            for(int i = 0; i < gridSize * gridSize; i++) {
                grid[i].subdivisediv(resolution);
            }
        }

        ImGui::Spacing();

        if(ImGui::SliderInt("Résolution de la grille", &gridSize, 1.0f, 25.0f)) {
            for(int i = 0; i < gridSize * gridSize; i++) {
                Plane plane(1, 512);
                plane.attachShader("../shaders/SinWave.vert", "../shaders/SinWave.frag");
                plane.createPlane();
                grid.push_back(plane);
            }
        }

        ImGui::Spacing();

        if (ImGui::Checkbox("Fil de fer", &fildefer)) {
            if (fildefer) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }

        ImGui::End();

        // Light
        glm::vec3 viewPos = cameraPos;

        // Render GUI window
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); 

        glfwSwapBuffers(window.get_window());
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    window.~Window();
    
    return 0;
}

void processInput(GLFWwindow *window) {
    // Camera sensitivity
    float camera_speed = 3.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) horizontal_angle -= camera_speed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) horizontal_angle += camera_speed;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) vertical_angle += camera_speed;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) vertical_angle -= camera_speed;

    //std::cout << cameraPos[0] << "\t" << cameraPos[1] << "\t" << cameraPos[2] << std::endl;

    // Ajuster le zoom
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) radius -= 10*camera_speed; // 10 fois pour que ça aille plus vite sans modifier la vitesse de l'orbite
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) radius += 10*camera_speed;

    // Limiter l'angle vertical
    vertical_angle = std::max(-1.57f, std::min(1.57f, vertical_angle));
}








/*
#include <Headers.hpp>

void processInput(GLFWwindow *window);
void initGUI(GLFWwindow* window);
void renderGUI();
void destroyGUI();

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

// Window settings
const unsigned int SCR_WIDTH = 1280;
const unsigned int SCR_HEIGHT = 720;

// Timing
float deltaTime = 0.0f;	
float lastFrame = 0.0f;

// Orbital camera settings
float horizontal_angle = 3.14f; 
float vertical_angle = 0.0f; 
float radius = 20.0f;

// Camera settings
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, -3.5f);
glm::vec3 cameraTarget = glm::vec3(0.0f, 0.0f,  0.0f);
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);


// Light settings (here for ImGui)
float ambientStrength = 0.1f;
float diffuseStrength = 1.0f;
float specularStrength = 0.5f;
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// // Sin settings (for plane)
float amplitude = 1.;
float x = 1.;
float k = 1.;
float af = 1.;
float p = 0.;

// Gerstner 
float L = 1.0;
float steepness = 1.0;

// Booléens modèles
bool ModeleSin = true;
bool ModeleGerstner = false;

// Booléen wireframe
bool fildefer = true;

int resolution = 512;

int main() {

    // Window
    Window window(4, 6, SCR_WIDTH, SCR_HEIGHT, "TER - OceanGL", true);
    window.setup_GLFW();

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.get_window(), true);
    ImGui_ImplOpenGL3_Init("#version 460");

    // Plane

    Plane plane(1, 512);
    plane.attachShader("../shaders/SinWave.vert", "../shaders/SinWave.frag");
    plane.createPlane();

    glEnable(GL_DEPTH_TEST);

    // Render loop
    while (!glfwWindowShouldClose(window.get_window())) {
        // Variables de la boucle
        float fps = ImGui::GetIO().Framerate;
        float ms_per_frame = 1000.0f / fps;
        float currentFrame = glfwGetTime();
        deltaTime = currentFrame - lastFrame;
        lastFrame = currentFrame;
        
        // Camera
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::lookAt(cameraPos,cameraTarget,cameraUp);
        glm::mat4 projection = glm::perspective(45.0f,(float)SCR_WIDTH/(float)SCR_HEIGHT,0.1f,500.0f);

        // Gestion des entrées
        processInput(window.get_window());

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // Imgui
        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

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

        ImGui::Text("Paramètres de la lumière :");
        ImGui::Spacing();

        ImGui::SliderFloat3("Position de la lumière", glm::value_ptr(lightPos), -10.0f, 10.0f);
        ImGui::SliderFloat("Ambiant", &ambientStrength, 0.0f, 1.0f);
        ImGui::SliderFloat("Diffus", &diffuseStrength, 0.0f, 2.0f);
        ImGui::SliderFloat("Spéculaire", &specularStrength, 0.0f, 1.0f);
        ImGui::Separator();


        ImGui::Spacing();

        if (ImGui::Checkbox("Modèle sinusoïdal", &ModeleSin)) {
            ModeleGerstner = false;

            
            plane.detachShader();
            plane.attachShader("../shaders/SinWave.vert", "../shaders/SinWave.frag");
            plane.createPlane();


        }
        
        ImGui::SameLine();

        if (ImGui::Checkbox("Modèle de Gerstner", &ModeleGerstner)) {
            ModeleSin = false;

            
            plane.detachShader();
            plane.attachShader("../shaders/GerstnerWave.vert", "../shaders/SinWave.frag");
            plane.createPlane();

  
        }

        ImGui::Spacing();

        if (ModeleSin) {
            
            plane.getShader().setBindMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
            plane.getShader().setBindMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
            plane.getShader().setBindMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
            plane.getShader().setBind1f("time",glfwGetTime());
            plane.getShader().setBind1f("amplitude", amplitude);
            plane.getShader().setBind1f("x", x);
            plane.getShader().setBind1f("k", k);
            plane.getShader().setBind1f("af", af);
            plane.getShader().setBind1f("p", p);

            ImGui::Text("Paramètres du shader sinusoïdal :");
            ImGui::SliderFloat("Amplitude", &amplitude, 0.0f, 20.0f);
            ImGui::SliderFloat("X", &x, 0.0f, 20.0f);
            ImGui::SliderFloat("K", &k, 0.0f, 20.0f);
            ImGui::SliderFloat("AF", &af, 0.0f, 20.0f);
            ImGui::SliderFloat("P", &p, 0.0f, 20.0f);
            ImGui::Separator();
        }
    
        if (ModeleGerstner) {

            plane.getShader().setBindMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
            plane.getShader().setBindMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
            plane.getShader().setBindMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
            plane.getShader().setBind1f("time",glfwGetTime());
            plane.getShader().setBind1f("amplitude", amplitude);
            plane.getShader().setBind1f("PI", M_PI);
            plane.getShader().setBind1f("g", 9.81); // constante gravitationelle
            plane.getShader().setBind1f("L", L);
            plane.getShader().setBind1f("steepness", steepness);

            ImGui::Text("Paramètres du shader Gerstner :");
            ImGui::SliderFloat("Amplitude", &amplitude, 0.0f, 20.0f);
            ImGui::SliderFloat("L", &L, 0.0f, 10.0f);
            ImGui::SliderFloat("Steepness", &steepness, 0.0f, 10.0f);
        }

        ImGui::Separator();

        ImGui::Spacing();

        ImGui::Text("Paramètres du plan :");
        if(ImGui::SliderInt("Résolution", &resolution, 0.0f, 2048.0f)) {
            plane.subdivisediv(resolution);
        }

        ImGui::Spacing();

        if (ImGui::Checkbox("Fil de fer", &fildefer)) {
            if (fildefer) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }

        ImGui::End();

        // Orbital camera
        
        cameraPos.x = cameraTarget.x + radius * cos(vertical_angle) * sin(horizontal_angle);
        cameraPos.y = cameraTarget.y + radius * sin(vertical_angle);
        cameraPos.z = cameraTarget.z + radius * cos(vertical_angle) * cos(horizontal_angle);        

        // Light
        glm::vec3 viewPos = cameraPos;


        plane.useShader();
        plane.updatePlane(GL_TRIANGLES);

        // Render GUI window
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); 

        glfwSwapBuffers(window.get_window());
        glfwPollEvents();
    }

    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    window.~Window();
    
    return 0;
}

void processInput(GLFWwindow *window) {
    // Camera sensitivity
    float camera_speed = 3.0f * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) horizontal_angle -= camera_speed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) horizontal_angle += camera_speed;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) vertical_angle += camera_speed;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) vertical_angle -= camera_speed;

    //std::cout << cameraPos[0] << "\t" << cameraPos[1] << "\t" << cameraPos[2] << std::endl;

    // Ajuster le zoom
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) radius -= 10*camera_speed; // 10 fois pour que ça aille plus vite sans modifier la vitesse de l'orbite
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) radius += 10*camera_speed;

    // Limiter l'angle vertical
    vertical_angle = std::max(-1.57f, std::min(1.57f, vertical_angle));
}
*/