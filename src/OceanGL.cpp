#include "imgui.h"
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
glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 10.0f);
// Initialement, la caméra regarde vers le centre du plan
glm::vec3 cameraTarget = glm::vec3(-0.0f, -5.0f, 10.0f); // Donc inverse de la position de caméra
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);

// Light settings (here for ImGui)
float ambientStrength = 0.1f;
float diffuseStrength = 1.0f;
float specularStrength = 0.5f;
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

// Sin
float Amplitude_Sin = 1.;   // amplitude
float L_Sin = 1.0;  // distance pic2crête
float S_Sin = 1.0;  // Vitesse
glm::vec3 Direction_Sin = glm::vec3(1.0f, 0.f, 0.f);
int numberWaves_Sin = 1;

// Gerstner 
float Amplitude_Gerstner = 1.0;
float L_Gerstner = 1.0;
float Steepness_Gerstner = 1.0;
float S_Gerstner = 1.0;
glm::vec3 Direction_Gerstner = glm::vec3(1.0f, 0.f, 0.f);

// SumSine
float Amplitude_SumSine = 1.0;
float Frequence_SumSine = 1.0;
int waveCount = 8;

// Booléens modèles
bool ModeleSin = true;
bool ModeleGerstner = false;
bool ModeleSumSine = false;


bool materiauSin = true;
bool positionSin = false;
bool uvSin = false;
bool normalSin = false;
bool binormalSin = false;
bool tangentSin = false;

bool materiauGerstner = true;
bool positionGerstner = false;
bool uvGerstner = false;
bool normalGerstner = false;
bool binormalGerstner = false;
bool tangentGerstner = false;

bool arretduTemps_Sin = false;
bool arretduTemps_Gerstner = false;
double temps = 0.;

// Booléen wireframe
bool fildefer = false;

// Booléen pour le déplacement de la caméra
bool cameraLibre = false;
// Vitesse de la caméra
int cameraSpeed = 1;

// Plan
int resolution = 512;
int taillePlan = 1;

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

    // Création de la skybox
    int skyboxSize = 2000;
    Skybox *sky = new Skybox(skyboxSize, glm::vec3(-1.*(skyboxSize/2),-1.*(skyboxSize/2),-1.*(skyboxSize/2)));
    sky->attachShader("../shaders/SkyboxVertex.vert", "../shaders/SkyboxFragment.frag");
    
    // Plane
    Plane plane(taillePlan, resolution);
    plane.attachShader("../shaders/SinWave.vert", "../shaders/SinWave.frag");
    plane.createPlane();

    // Chargement des textures
    GLint nxSkybox = loadTexture2DFromFilePath("../Textures/Skybox/nx.png");
    GLint nySkybox = loadTexture2DFromFilePath("../Textures/Skybox/ny.png");
    GLint nzSkybox = loadTexture2DFromFilePath("../Textures/Skybox/nz.png");
    GLint pxSkybox = loadTexture2DFromFilePath("../Textures/Skybox/px.png");
    GLint pySkybox = loadTexture2DFromFilePath("../Textures/Skybox/py.png");
    GLint pzSkybox = loadTexture2DFromFilePath("../Textures/Skybox/pz.png");

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
        if (!cameraLibre){
            cameraTarget = -1.0f * cameraPos;
        }
        glm::mat4 model = glm::mat4(1.0f);
        glm::mat4 view = glm::lookAt(cameraPos,cameraPos + cameraTarget,cameraUp);
        glm::mat4 projection = glm::perspective(45.0f,(float)SCR_WIDTH/(float)SCR_HEIGHT,0.1f,2000.0f);

        // Gestion des entrées
        processInput(window.get_window());

        glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
        glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // ImGui
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
            ModeleSumSine = false;

            plane.detachShader();
            plane.attachShader("../shaders/SinWave.vert", "../shaders/SinWave.frag");
            plane.createPlane();
        }
        
        ImGui::SameLine();

        if (ImGui::Checkbox("Modèle de Gerstner", &ModeleGerstner)) {
            ModeleSin = false;
            ModeleSumSine = false;

            plane.detachShader();
            plane.attachShader("../shaders/GerstnerWave.vert", "../shaders/GerstnerWave.frag");
            plane.createPlane();    
        }

        ImGui::SameLine();

        if (ImGui::Checkbox("Modèle avec plusieurs sinusoïdales", &ModeleSumSine)) {
            ModeleSin = false;
            ModeleGerstner = false;

            sky->detachShader();
            sky->attachShader("../shaders/SkyboxVertex.vert", "../shaders/SkyboxFragment.frag");
            
            plane.detachShader();
            plane.attachShader("../shaders/SumSine.vert", "../shaders/SumSine.frag");
            plane.createPlane();    
        }

        ImGui::Spacing();

        if (ModeleSin) {
                plane.getShader().setBindMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
                plane.getShader().setBindMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
                plane.getShader().setBindMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
                plane.getShader().setBind1f("time", glfwGetTime());
                plane.getShader().setBind1f("Amplitude", Amplitude_Sin);
                plane.getShader().setBind1f("L", L_Sin);
                plane.getShader().setBind1f("S", S_Sin);
                plane.getShader().setBind3f("Direction", Direction_Sin.x, Direction_Sin.y, Direction_Sin.z);

                if(materiauSin == true) {
                    plane.getShader().setBind1i("Debug", 0);
                } else if(positionSin == true) {
                    plane.getShader().setBind1i("Debug", 1);
                } else if(uvSin == true) {
                    plane.getShader().setBind1i("Debug", 2);
                } else if(normalSin == true) {
                    plane.getShader().setBind1i("Debug", 3);
                } else if(binormalSin == true) {
                    plane.getShader().setBind1i("Debug", 4);
                } else if(tangentSin == true) {
                    plane.getShader().setBind1i("Debug", 5);
                }
                
                plane.useShader();
                plane.updatePlane(GL_TRIANGLES);     
            

            ImGui::Text("Paramètres du shader sinusoïdal :");
            ImGui::SliderFloat("Amplitude", &Amplitude_Sin, 0.0f, 20.0f);
            ImGui::SliderFloat("L", &L_Sin, 0.0f, 10.0f);

            if(ImGui::SliderFloat("S", &S_Sin, 0.0f, 10.0f) != 0.) {
                arretduTemps_Sin = false;
            }

            ImGui::SliderFloat("X", &Direction_Sin.x, -1.0f, 1.0f);
            ImGui::SliderFloat("Z", &Direction_Sin.z, -1.0f, 1.0f);

            ImGui::Separator();
            ImGui::Text("Debug du shader");

            if (ImGui::Checkbox("Matériau", &materiauSin)) {
                if (materiauSin) {
                    positionSin = false;
                    uvSin = false;
                    normalSin = false;
                    binormalSin = false;
                    tangentSin = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Position", &positionSin)) {
                if (positionSin) {
                    materiauSin = false;
                    uvSin = false;
                    normalSin = false;
                    binormalSin = false;
                    tangentSin = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("UV", &uvSin)) {
                if (uvSin) {
                    materiauSin = false;
                    positionSin = false;
                    normalSin = false;
                    binormalSin = false;
                    tangentSin = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Normale", &normalSin)) {
                if (normalSin) {
                    materiauSin = false;
                    positionSin = false;
                    uvSin = false;
                    binormalSin = false;
                    tangentSin = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Binormale", &binormalSin)) {
                if (binormalSin) {
                    materiauSin = false;
                    positionSin = false;
                    uvSin = false;
                    normalSin = false;
                    tangentSin = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Tangente", &tangentSin)) {
                if (tangentSin) {
                    materiauSin = false;
                    positionSin = false;
                    uvSin = false;
                    normalSin = false;
                    binormalSin = false;
                }
            }

            if (ImGui::Checkbox("Arrêt du temps", &arretduTemps_Sin)) {
                float S_Sin_save = S_Sin;
                if (arretduTemps_Sin) {
                    S_Sin = 0.0; 
                } else {
                    S_Sin = 1.0;
                }
            }

            ImGui::Separator();
        }

        if (ModeleGerstner) {
                plane.getShader().setBindMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
                plane.getShader().setBindMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));                  
                plane.getShader().setBindMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
                plane.getShader().setBind1f("time", glfwGetTime());
                plane.getShader().setBind1f("Amplitude", Amplitude_Gerstner);
                plane.getShader().setBind1f("PI", M_PI);
                plane.getShader().setBind1f("g", 9.81); // Accélération de la pesanteur
                plane.getShader().setBind1f("L", L_Gerstner);
                plane.getShader().setBind1f("S", S_Gerstner);
                plane.getShader().setBind1f("Steepness", Steepness_Gerstner);
                plane.getShader().setBind3f("Direction", Direction_Gerstner.x, Direction_Gerstner.y, Direction_Gerstner.z);

                if(materiauGerstner == true) {
                    plane.getShader().setBind1i("Debug", 0);
                } else if(positionGerstner == true) {
                    plane.getShader().setBind1i("Debug", 1);
                } else if(uvGerstner == true) {
                    plane.getShader().setBind1i("Debug", 2);
                } else if(normalGerstner == true) {
                    plane.getShader().setBind1i("Debug", 3);
                } else if(binormalGerstner == true) {
                    plane.getShader().setBind1i("Debug", 4);
                } else if(tangentGerstner == true) {
                    plane.getShader().setBind1i("Debug", 5);
                }

                plane.useShader();
                plane.updatePlane(GL_TRIANGLES);
                

            ImGui::Text("Paramètres du shader Gerstner :");
            ImGui::SliderFloat("Amplitude", &Amplitude_Gerstner, 0.0f, 20.0f);
            ImGui::SliderFloat("L", &L_Gerstner, 0.0f, 10.0f);
            ImGui::SliderFloat("Steepness", &Steepness_Gerstner, 0.0f, 10.0f);
            
            if(ImGui::SliderFloat("S", &S_Gerstner, 0.0f, 10.0f) != 0.) {
                arretduTemps_Gerstner = false;
            }

            ImGui::SliderFloat("X", &Direction_Gerstner.x, -1.0f, 1.0f);
            ImGui::SliderFloat("Z", &Direction_Gerstner.z, -1.0f, 1.0f);

            ImGui::Separator();
            ImGui::Text("Debug du shader");

            if (ImGui::Checkbox("Material", &materiauGerstner)) {
                if (materiauGerstner) {
                    positionGerstner = false;
                    uvGerstner = false;
                    normalGerstner = false;
                    binormalGerstner = false;
                    tangentGerstner = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Position", &positionGerstner)) {
                if (positionGerstner) {
                    materiauGerstner = false;
                    uvGerstner = false;
                    normalGerstner = false;
                    binormalGerstner = false;
                    tangentGerstner = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("UV", &uvGerstner)) {
                if (uvGerstner) {
                    materiauGerstner = false;
                    positionGerstner = false;
                    normalGerstner = false;
                    binormalGerstner = false;
                    tangentGerstner = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Normal", &normalGerstner)) {
                if (normalGerstner) {
                    materiauGerstner = false;
                    positionGerstner = false;
                    uvGerstner = false;
                    binormalGerstner = false;
                    tangentGerstner = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Binormale", &binormalGerstner)) {
                if (binormalGerstner) {
                    materiauGerstner = false;
                    positionGerstner = false;
                    uvGerstner = false;
                    normalGerstner = false;
                    tangentGerstner = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Tangente", &tangentGerstner)) {
                if (tangentGerstner) {
                    materiauGerstner = false;
                    positionGerstner = false;
                    uvGerstner = false;
                    normalGerstner = false;
                    binormalGerstner = false;
                }
            }

            if (ImGui::Checkbox("Arrêt du temps", &arretduTemps_Gerstner)) {
                float S_Gerstner_save = S_Gerstner;
                if (arretduTemps_Gerstner) {
                    S_Gerstner = 0.0;
                } else {
                    S_Gerstner = 1.0;
                }
            }

            ImGui::Separator();
        }

        if (ModeleSumSine) {
            plane.getShader().setBindMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
            plane.getShader().setBindMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
            plane.getShader().setBindMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
            plane.getShader().setBind1f("time", glfwGetTime());
            plane.getShader().setBind1f("amplitude", Amplitude_SumSine);
            plane.getShader().setBind1f("frequence", Frequence_SumSine);
                
            plane.getShader().setBind1i("nxTexture", nxSkybox);
            plane.getShader().setBind1i("nyTexture", nySkybox);
            plane.getShader().setBind1i("nzTexture", nzSkybox);
            plane.getShader().setBind1i("pxTexture", pxSkybox);
            plane.getShader().setBind1i("pyTexture", pySkybox);
            plane.getShader().setBind1i("pzTexture", pzSkybox);


            if(materiauSin == true) {
                plane.getShader().setBind1i("Debug", 0);
            } else if(positionSin == true) {
                plane.getShader().setBind1i("Debug", 1);
            } else if(uvSin == true) {
                plane.getShader().setBind1i("Debug", 2);
            } else if(normalSin == true) {
                plane.getShader().setBind1i("Debug", 3);
            } else if(binormalSin == true) {
                plane.getShader().setBind1i("Debug", 4);
            } else if(tangentSin == true) {
                plane.getShader().setBind1i("Debug", 5);
            }

            //plane.useShader();
            //plane.updatePlane(GL_TRIANGLES);

            sky->useShader();
            sky->updateSkybox(GL_TRIANGLES);


            ImGui::Text("Paramètres du shader SumSine :");
            ImGui::SliderFloat("Amplitude", &Amplitude_SumSine, 0.0f, 20.0f);
            ImGui::SliderFloat("Fréquence", &Frequence_SumSine, 0.0f, 20.0f);
        }

        ImGui::Spacing();

        ImGui::Text("Paramètres du plan :");
        if(ImGui::SliderInt("Résolution", &resolution, 0.0f, 2048.0f)) {
            plane.subdivisediv(resolution);
        }

        ImGui::Spacing();

        if(ImGui::SliderInt("Résolution de la grille", &taillePlan, 1, 50)) {
            plane.updateSize(taillePlan);
        }

        ImGui::Spacing();

        if (ImGui::Checkbox("Fil de fer", &fildefer)) {
            if (fildefer) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }

        ImGui::Separator();

        ImGui::Checkbox("Caméra libre", &cameraLibre);

        ImGui::Spacing();

        ImGui::SliderInt("Vitesse caméra", &cameraSpeed, 1, 10);

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

    /*
    // Ajuster le zoom
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) radius -= 10*camera_speed; // 10 fois pour que ça aille plus vite sans modifier la vitesse de l'orbite
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) radius += 10*camera_speed;
    */

    // Avancer/Reculer la caméra
    if (glfwGetKey(window, GLFW_KEY_R) == GLFW_PRESS){
        cameraPos += (cameraSpeed / 100.f) * cameraTarget;
    }
    if (glfwGetKey(window, GLFW_KEY_F) == GLFW_PRESS){
        cameraPos -= (cameraSpeed / 100.f) * cameraTarget;
    }

    // Monter/Descendre la caméra
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        cameraPos += (cameraSpeed / 10.f) * cameraUp;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        cameraPos -= (cameraSpeed / 10.f) * cameraUp;
    }

    // Déplacer vers la droite/gauche la caméra
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        cameraPos += (cameraSpeed / 10.f) * glm::normalize(glm::cross(cameraTarget,cameraUp));;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        cameraPos -= (cameraSpeed / 10.f) * glm::normalize(glm::cross(cameraTarget,cameraUp));;
    }

    // Limiter l'angle vertical
    vertical_angle = std::max(-1.57f, std::min(1.57f, vertical_angle));
}