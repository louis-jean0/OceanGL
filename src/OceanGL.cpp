#include "imgui.h"
#include <Headers.hpp>

//#define MINIAUDIO_IMPLEMENTATION
#include "miniaudio.h"

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
int SCR_WIDTH = 1920;
int SCR_HEIGHT = 1080;

Flotability_2 *manageFlotability;
bool objetsApparition = false;
float heightSpawn = 0.0f;

Sound *soundManager;

// Timing
float deltaTime = 0.0f;	
float lastFrame = 0.0f;

// Orbital camera settings
float horizontal_angle = 3.14f; 
float vertical_angle = 0.0f; 
float radius = 20.0f;

// Camera settings
glm::vec3 cameraPos = glm::vec3(0.0f, 5.0f, 5.0f);
glm::vec3 cameraTarget;;
glm::vec3 cameraUp = glm::vec3(0.0f, 1.0f,  0.0f);
int speedCam = 30;
bool cameraMouse = true;

double previousX = SCR_WIDTH / 2;
double previousY = SCR_HEIGHT / 2;
bool firstMouse = true;
float phi = -90.0f;
float theta = 0.0f;

// Light settings (here for ImGui)
glm::vec3 LightPos = glm::vec3(0., 0., 0.);
glm::vec3 ViewPos = cameraPos;
glm::vec3 LightColor = glm::vec3(0.2,0.2,0.85);
float ambientStrength = 0.1f;
float diffuseStrength = 0.1f;
float specularStrength = 0.1f;
float roughness = 0.5f;
float ao = 0.5f;
float metallic = 0.5f;
glm::vec3 albedo = glm::vec3(1., 0., 0.);

// Sin
float Amplitude_Sin = 0.5;   // amplitude
float L_Sin = 1.0;  // distance pic2crête
float S_Sin = 1.0;  // Vitesse
glm::vec3 Direction_Sin = glm::vec3(1.0f, 0.f, 0.f);

// Gerstner 
float Amplitude_Gerstner = 0.5;
float L_Gerstner = 3.0;
float S_Gerstner = 1.0;
float Steepness_Gerstner = 1.0;
glm::vec3 Direction_Gerstner = glm::vec3(1.0f, 0.f, 0.f);

// SumSin
// Généraux
int numWave_SumSines = 30;
int seed = 0;
bool FBM_SumSines = true;
bool DomainWarping_SumSines = true;

// Sans FBM
float Amplitude_SumSines_min = 0.1; 
float Amplitude_SumSines_max = 1.;  
float L_min_SumSines = 0.5;  
float L_max_SumSines = 1.0;
float S_SumSines = 1.0;
glm::vec3 Direction_SumSines = glm::vec3(1.0f, 0.f, 0.f);

// Avec FBM
float Amplitude_SumSines_FBM = 0.5;
float Gain_A_SumSines = 0.7;
float Gain_W_SumSines = 1.2;
float L_FBM_SumSines = 18.0;

// SumGerstner
// Généraux
float Amplitude_SumGerstner_min = 0.1;
float Amplitude_SumGerstner_max = 1.0;

float L_SumGerstner_min = 1.0;
float L_SumGerstner_max = 1.0;
float Steepness_SumGerstner = 0.34;
float S_SumGerstner = 20.0;
glm::vec3 Direction_SumGerstner = glm::vec3(1.0f, 0.f, 0.f);
int numWave_SumGerstner = 50;

// Avec FBM
float Amplitude_SumGerstner_FBM = 0.35;
float Gain_A_SumGerstner = 0.54;
float Gain_W_SumGerstner = 1.5;
float L_FBM_SumGerstner = 25;
bool FBM_SumGerstner = true;

// Booléens modèles
bool ModeleSin = true;
bool ModeleGerstner = false;
bool ModeleSumSine = false;
bool ModeleSumGerstner = false;
bool ModeleFFT = false;

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

bool materiauSumSines = true;
bool positionSumSines = false;
bool uvSumSines = false;
bool normalSumSines = false;
bool binormalSumSines = false;
bool tangentSumSines = false;

bool materiauSumGerstner = true;
bool positionSumGerstner = false;
bool uvSumGerstner = false;
bool normalSumGerstner = false;
bool binormalSumGerstner = false;
bool tangentSumGerstner = false;


bool arretduTemps_Sin = false;
bool arretduTemps_Gerstner = false;
bool arretduTemps_SumSines = false;
bool arretduTemps_SumGerstner = false;

double temps = 0.;

// Booléen wireframe
bool fildefer = false;
bool points = false;

// Plan
int resolution = 512;
int taillePlan = 10;

// Skybox
int resolutionSkybox = 5000;

// Compute Shader
int tailleImage = 512;
int nbThreads = 16;

// Textures
Texture test;

void framebuffer_size_callback(GLFWwindow* window, int width, int height);

void mouse_cursor_callback(GLFWwindow* window, double xpos, double ypos){
    if (cameraMouse){
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED); // Pour masquer la souris sur la fenêtre

        if (firstMouse){
            previousX = xpos;
            previousY = ypos;
            firstMouse = false;
        }

        double deltaX = xpos - previousX;
        double deltaY = previousY - ypos;

        phi += deltaX*0.05f;
        theta += deltaY*0.05f;
        // On empêche la caméra de s'inverser
        theta = std::max(std::min(theta, 89.99f), -89.99f);

        float x = cos(glm::radians(phi)) * cos(glm::radians(theta));
        float y = sin(glm::radians(theta));
        float z = sin(glm::radians(phi)) * cos(glm::radians(theta));
        cameraTarget = glm::normalize(glm::vec3(x,y,z));

        previousX=xpos;
        previousY=ypos;
    } else {
        glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
    }
}

void soundEffectEndCallback(void* pUserData, ma_sound* pSound) {
    soundManager->setIsEffectPlayed(false);
}

void backgroundSoundCallback(void* pUserData, ma_sound* pSound) {
    soundManager->setIsBackgroundPlayed(false);
}

int main() {
    
    // Window
    Window window(4, 6, SCR_WIDTH, SCR_HEIGHT, "TER - OceanGL", true);
    window.setup_GLFW();

    glfwSetCursorPosCallback(window.get_window(), mouse_cursor_callback);
    glfwSetFramebufferSizeCallback(window.get_window(), framebuffer_size_callback);

    glfwGetFramebufferSize(window.get_window(), &SCR_WIDTH, &SCR_HEIGHT);
    framebuffer_size_callback(window.get_window(), SCR_WIDTH, SCR_HEIGHT);

    // Initialize ImGui
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    ImGui::StyleColorsDark();
    ImGui_ImplGlfw_InitForOpenGL(window.get_window(), true);
    ImGui_ImplOpenGL3_Init("#version 460");

    // Plane
    Plane plane(taillePlan, resolution);
    plane.attachShader("../shaders/SinWave.vert", "../shaders/SinWave.frag");
    plane.createPlane();
    plane.loadCubemap();

    Skybox skybox;
    skybox.attachShader("../shaders/skybox_vertex.vert", "../shaders/skybox_fragment.frag");
    skybox.createSkybox();
    skybox.loadCubemap();

    glEnable(GL_DEPTH_TEST);
    
    manageFlotability = new Flotability_2(heightSpawn);
    
    soundManager = new Sound();
    ma_sound_set_end_callback(soundManager->getSeagull1(), soundEffectEndCallback, nullptr);
    ma_sound_set_end_callback(soundManager->getSeagull2(), soundEffectEndCallback, nullptr);
    ma_sound_set_end_callback(soundManager->getBackgroundSound(), backgroundSoundCallback, nullptr);

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
        glm::mat4 modelSkybox = glm::mat4(1.0f);
        glm::mat4 view = glm::lookAt(cameraPos, cameraPos + cameraTarget, cameraUp);
        glm::mat4 projection = glm::perspective(45.0f,(float)SCR_WIDTH/(float)SCR_HEIGHT, 0.1f, 5000000.0f);

        // Gestion des entrées
        processInput(window.get_window());

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
        
        // ---------------------------------------------------------------------
        if (!soundManager->getEffectPlayed()){
            int n = rand() % 500;
            if (n==0){
                soundManager->playSeagull1();
            }else if (n==1){
                soundManager->playSeagull2();
            }
        }
        if (!soundManager->getBackgroundPlayed()){
            soundManager->playBackgroundSound();
        }
        // ---------------------------------------------------------------------

        if(ModeleSin) {
            ImGui::Text("Paramètres de la lumière :");
            ImGui::Spacing();
            ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f);
            ImGui::SliderFloat("Occlusion ambiante", &ao, 0.0f, 1.0f);
            ImGui::SliderFloat("Métallique", &metallic, 0.0f, 1.0f);
            ImGui::ColorEdit3("Albedo", &albedo[0]);
            ImGui::Text("Position de la lumière");
            ImGui::SliderFloat("X_lightpos", &LightPos.x, -10.0f, 10.0f);
            ImGui::SliderFloat("Y_lightpos", &LightPos.y, -10.0f, 10.0f);
            ImGui::SliderFloat("Z_lightpos", &LightPos.z, -10.0f, 10.0f);
            ImGui::Separator();
        }

        if(ModeleGerstner) {
            ImGui::Text("Paramètres de la lumière :");
            ImGui::Spacing();
            ImGui::SliderFloat("Roughness", &roughness, 0.0f, 1.0f);
            ImGui::SliderFloat("Occlusion ambiante", &ao, 0.0f, 1.0f);
            ImGui::SliderFloat("Métallique", &metallic, 0.0f, 1.0f);
            ImGui::ColorEdit3("Albedo", &albedo[0]);
            ImGui::Text("Position de la lumière");
            ImGui::SliderFloat("X_lightpos", &LightPos.x, -10.0f, 10.0f);
            ImGui::SliderFloat("Y_lightpos", &LightPos.y, -10.0f, 10.0f);
            ImGui::SliderFloat("Z_lightpos", &LightPos.z, -10.0f, 10.0f);
            ImGui::Separator();
        }

        if(ModeleSumSine) {
            ImGui::Text("Paramètres de la lumière :");
            ImGui::Spacing();
            ImGui::SliderFloat("Ambient", &ambientStrength, 0.0f, 1.0f);
            ImGui::SliderFloat("Diffuse", &diffuseStrength, 0.0f, 1.0f);
            ImGui::SliderFloat("Specular", &specularStrength, 0.0f, 1.0f);
            ImGui::ColorEdit3("Couleur de la lumière", &LightColor[0]);
            ImGui::Text("Position de la lumière :");
            ImGui::SliderFloat("X", &LightPos.x, -10.0f, 10.0f);
            ImGui::SliderFloat("Y", &LightPos.y, -10.0f, 10.0f);
            ImGui::SliderFloat("Z", &LightPos.z, -10.0f, 10.0f);
            ImGui::Separator();
        }

        if(ModeleSumGerstner) {
            ImGui::Text("Paramètres de la lumière :");
            ImGui::Spacing();
            ImGui::SliderFloat("Ambient", &ambientStrength, 0.0f, 1.0f);
            ImGui::SliderFloat("Diffuse", &diffuseStrength, 0.0f, 1.0f);
            ImGui::SliderFloat("Specular", &specularStrength, 0.0f, 1.0f);
            ImGui::ColorEdit3("Couleur de la lumière", &LightColor[0]);
            ImGui::Text("Position de la lumière :");
            ImGui::SliderFloat("X", &LightPos.x, -10.0f, 10.0f);
            ImGui::SliderFloat("Y", &LightPos.y, -10.0f, 10.0f);
            ImGui::SliderFloat("Z", &LightPos.z, -10.0f, 10.0f);
            ImGui::Separator();
        }

        ImGui::Spacing();

        if (ImGui::Checkbox("Modèle sinusoïdal", &ModeleSin)) {
            ModeleGerstner = false;
            ModeleSumSine = false;
            ModeleSumGerstner = false;
            ModeleFFT = false;

            plane.detachShader();
            plane.attachShader("../shaders/SinWave.vert", "../shaders/SinWave.frag");
            plane.createPlane();
        }
        
        ImGui::SameLine();

        if (ImGui::Checkbox("Modèle de Gerstner", &ModeleGerstner)) {
            ModeleSin = false;
            ModeleSumSine = false;
            ModeleSumGerstner = false;
            ModeleFFT = false;

            plane.detachShader();
            plane.attachShader("../shaders/GerstnerWave.vert", "../shaders/GerstnerWave.frag");
            plane.createPlane();    
        }

        ImGui::SameLine();

        if (ImGui::Checkbox("Modèle avec plusieurs sinusoïdales", &ModeleSumSine)) {
            ModeleSin = false;
            ModeleGerstner = false;
            ModeleSumGerstner = false;
            ModeleFFT = false;

            plane.detachShader();
            plane.attachShader("../shaders/SumSinWave.vert", "../shaders/SumSinWave.frag");
            plane.createPlane();    
        }

        if (ImGui::Checkbox("Modèle avec plusieurs vagues de Gerstner", &ModeleSumGerstner)) {
            ModeleSin = false;
            ModeleGerstner = false;
            ModeleSumSine = false;
            ModeleFFT = false;

            plane.detachShader();
            plane.attachShader("../shaders/SumGerstnerWave.vert", "../shaders/SumGerstnerWave.frag");
            plane.createPlane();    
        }

        if (ImGui::Checkbox("Modèle FFT", &ModeleFFT)) {
            ModeleSin = false;
            ModeleSumSine = false;
            ModeleGerstner = false;
            ModeleSumGerstner = false;

            plane.ComputeWorkGroup();

            plane.detachShader();
            plane.attachShader("../shaders/FFT.vert", "../shaders/FFT.frag");
            plane.attachShaderComp("../shaders/comp.cs");
            plane.createPlane();
            //test.setTexture("../textures/grass.png", "asaz", true);
            test.createTexture(tailleImage, tailleImage);
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
            plane.getShader().setBind1f("PI", M_PI);

            // PBR
            plane.getShader().setBind3f("LightPos", LightPos.x, LightPos.y, LightPos.z);
            plane.getShader().setBind3f("ViewPos", cameraPos.x, cameraPos.y, cameraPos.z);
            plane.getShader().setBind1f("roughness", roughness);
            plane.getShader().setBind1f("ao", ao);
            plane.getShader().setBind1f("metallic", metallic);
            plane.getShader().setBind3f("albedo", albedo.x, albedo.y, albedo.z);

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
                
            skybox.useShader();
            skybox.getShader().setBindMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(modelSkybox));
            skybox.getShader().setBindMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
            skybox.getShader().setBindMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
            skybox.getShader().setBind1i("res", resolutionSkybox);
            skybox.bindCubemap(GL_TEXTURE0,0);
            skybox.updateSkybox(GL_TRIANGLES);
            
            // Affichage des objets flottants
            if (objetsApparition){
                manageFlotability->drawSphere(deltaTime,model,view,projection,Amplitude_Sin, L_Sin, S_Sin);
            }


            plane.useShader();
            plane.bindCubemap(GL_TEXTURE1,1);
            plane.updatePlane(GL_TRIANGLES); 
            

            ImGui::Text("Paramètres du shader sinusoïdal :");
            ImGui::SliderFloat("Amplitude", &Amplitude_Sin, 0.0f, 5.0f);
            ImGui::SliderFloat("Wavelength", &L_Sin, 0.01f, 10.0f);

            if(ImGui::SliderFloat("Speed", &S_Sin, 0.0f, 15.0f) != 0.) {
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
            plane.getShader().setBind1f("Wavelength", L_Gerstner);
            plane.getShader().setBind1f("Steepness", Steepness_Gerstner);
            plane.getShader().setBind1f("Speed", S_Gerstner);
            plane.getShader().setBind3f("Direction", Direction_Gerstner.x, Direction_Gerstner.y, Direction_Gerstner.z);
            plane.getShader().setBind3f("lightPosition", LightPos.x, LightPos.y, LightPos.z);
            plane.getShader().setBind3f("viewPosition",cameraPos.x,cameraPos.y,cameraPos.z);
            plane.getShader().setBind3f("lightColor", LightColor.x, LightColor.y, LightColor.z);
            plane.getShader().setBind1f("ambientStrength", ambientStrength);
            plane.getShader().setBind1f("diffuseStrength", diffuseStrength);
            plane.getShader().setBind1f("specularStrength", specularStrength);

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

            skybox.useShader();
            skybox.getShader().setBindMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(modelSkybox));
            skybox.getShader().setBindMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
            skybox.getShader().setBindMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
            skybox.getShader().setBind1i("res", resolutionSkybox);
            skybox.bindCubemap(GL_TEXTURE0,0);
            skybox.updateSkybox(GL_TRIANGLES);
            
            // Affichage des objets flottants
            if (objetsApparition){
                manageFlotability->drawSphere(deltaTime,model,view,projection,Amplitude_Sin, L_Sin, S_Sin);
            }


            plane.useShader();
            plane.bindCubemap(GL_TEXTURE1,1);
            plane.updatePlane(GL_TRIANGLES); 

            ImGui::Text("Paramètres du shader Gerstner :");
            ImGui::SliderFloat("Amplitude", &Amplitude_Gerstner, 0.01f, 4.0f);
            ImGui::SliderFloat("Wavelength", &L_Gerstner, 0.01f, 30.0f);
            ImGui::SliderFloat("Steepness", &Steepness_Gerstner, 0.0f, 1.0f);

            if(ImGui::SliderFloat("Speed", &S_Gerstner, 0.0f, 30.0f) != 0.) {
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
            plane.getShader().setBind1f("Amplitude_min", Amplitude_SumSines_min);
            plane.getShader().setBind1f("Amplitude_max", Amplitude_SumSines_max);
            plane.getShader().setBind1f("L_min", L_min_SumSines);
            plane.getShader().setBind1f("L_max", L_max_SumSines);
            plane.getShader().setBind1f("S", S_SumSines);
            plane.getShader().setBind3f("Direction", Direction_SumSines.x, Direction_SumSines.y, Direction_SumSines.z);
            plane.getShader().setBind1f("PI", M_PI);
            plane.getShader().setBind1f("Amplitude_FBM", Amplitude_SumSines_FBM);
            plane.getShader().setBind1f("Gain_A", Gain_A_SumSines);
            plane.getShader().setBind1f("Gain_W", Gain_W_SumSines);
            plane.getShader().setBind1f("L_FBM", L_FBM_SumSines);
            plane.getShader().setBind3f("lightPosition", LightPos.x, LightPos.y, LightPos.z);
            plane.getShader().setBind3f("viewPosition",cameraPos.x,cameraPos.y,cameraPos.z);
            plane.getShader().setBind3f("lightColor", LightColor.x, LightColor.y, LightColor.z);
            plane.getShader().setBind1f("ambientStrength", ambientStrength);
            plane.getShader().setBind1f("diffuseStrength", diffuseStrength);
            plane.getShader().setBind1f("specularStrength", specularStrength);

            if(materiauSumSines == true) {
                plane.getShader().setBind1i("Debug", 0);
            } else if(positionSumSines == true) {
                plane.getShader().setBind1i("Debug", 1);
            } else if(uvSumSines == true) {
                plane.getShader().setBind1i("Debug", 2);
            } else if(normalSumSines == true) {
                plane.getShader().setBind1i("Debug", 3);
            } else if(binormalSumSines == true) {
                plane.getShader().setBind1i("Debug", 4);
            } else if(tangentSumSines == true) {
                plane.getShader().setBind1i("Debug", 5);
            }

            plane.getShader().setBind1i("nbVagues", numWave_SumSines);
            plane.getShader().setBind1i("seed", seed);

            if(FBM_SumSines == false) {
                plane.getShader().setBind1i("FBM", 0);
            } else {
                plane.getShader().setBind1i("FBM", 1);
            }

            if(DomainWarping_SumSines == false) {
                plane.getShader().setBind1i("DW", 0);
            } else {
                plane.getShader().setBind1i("DW", 1);
            }

            skybox.useShader();
            skybox.getShader().setBindMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(modelSkybox));
            skybox.getShader().setBindMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
            skybox.getShader().setBindMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
            skybox.getShader().setBind1i("res", resolutionSkybox);
            skybox.bindCubemap(GL_TEXTURE0,0);
            skybox.updateSkybox(GL_TRIANGLES);
            
            // Affichage des objets flottants
            if (objetsApparition){
                manageFlotability->drawSphere(deltaTime,model,view,projection,Amplitude_Sin, L_Sin, S_Sin);
            }


            plane.useShader();
            plane.bindCubemap(GL_TEXTURE1,1);
            plane.updatePlane(GL_TRIANGLES); 

            ImGui::Text("Paramètres du shader SumSine :");

            if(FBM_SumSines == false) {
                ImGui::SliderFloat("Amplitude min", &Amplitude_SumSines_min, 0.0f, 2.0f);
                ImGui::SliderFloat("Amplitude max", &Amplitude_SumSines_max, 0.0f, 2.0f);
                ImGui::SliderFloat("Wavelength min", &L_min_SumSines, 0.01f, 10.0f);
                ImGui::SliderFloat("Wavelength max", &L_max_SumSines, 0.01f, 10.0f);
                if(ImGui::SliderFloat("Speed", &S_SumSines, 0.0f, 10.0f) != 0.) {
                    arretduTemps_SumSines = false;
                }
                ImGui::SliderFloat("X", &Direction_SumSines.x, -1.0f, 1.0f);
                ImGui::SliderFloat("Z", &Direction_SumSines.z, -1.0f, 1.0f);         
            } else {
                ImGui::SliderFloat("Amplitude", &Amplitude_SumSines_FBM, 0.0f, 2.0f);
                if(ImGui::SliderFloat("Speed", &S_SumSines, 0.0f, 10.0f) != 0.) {
                    arretduTemps_SumSines = false;
                }
                ImGui::SliderFloat("Gain amplitude", &Gain_A_SumSines, 0.01f, 0.99f);
                ImGui::SliderFloat("Gain wavelength", &Gain_W_SumSines, 0.01, 5.0f);
                ImGui::SliderFloat("Wavelength", &L_FBM_SumSines, 0.01f, 50.0f);
            }

            ImGui::SliderInt("Nombre de vagues", &numWave_SumSines, 1, 256);
            ImGui::SliderInt("Seed", &seed, 0, 25);
            ImGui::Checkbox("Fractionnal Brownian Motion", &FBM_SumSines);
            if(FBM_SumSines == true) {
                ImGui::Checkbox("Domain Warping", &DomainWarping_SumSines);
            }

            ImGui::Separator();
            ImGui::Text("Debug du shader");

            if (ImGui::Checkbox("Material", &materiauSumSines)) {
                if (materiauSumSines) {
                    positionSumSines= false;
                    uvSumSines = false;
                    normalSumSines = false;
                    binormalSumSines = false;
                    tangentSumSines = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Position", &positionSumSines)) {
                if (positionSumSines) {
                    materiauSumSines = false;
                    uvSumSines = false;
                    normalSumSines = false;
                    binormalSumSines = false;
                    tangentSumSines = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("UV", &uvSumSines)) {
                if (uvSumSines) {
                    materiauSumSines = false;
                    positionSumSines = false;
                    normalSumSines = false;
                    binormalSumSines = false;
                    tangentSumSines = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Normal", &normalSumSines)) {
                if (normalSumSines) {
                    materiauSumSines = false;
                    positionSumSines = false;
                    uvSumSines = false;
                    binormalSumSines = false;
                    tangentSumSines = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Binormale", &binormalSumSines)) {
                if (binormalSumSines) {
                    materiauSumSines = false;
                    positionSumSines = false;
                    uvSumSines = false;
                    normalSumSines = false;
                    tangentSumSines = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Tangente", &tangentSumSines)) {
                if (tangentSumSines) {
                    materiauSumSines = false;
                    positionSumSines = false;
                    uvSumSines = false;
                    normalSumSines = false;
                    binormalSumSines = false;
                }
            }

            if (ImGui::Checkbox("Arrêt du temps", &arretduTemps_SumSines)) {
                float S_SumSines_save = S_SumSines;
                if (arretduTemps_SumSines) {
                    S_SumSines = 0.0;
                } else {
                    S_SumSines = 1.0;
                }
            }

            ImGui::Separator();
        }
        if (ModeleSumGerstner) {
            plane.getShader().setBindMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
            plane.getShader().setBindMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));                  
            plane.getShader().setBindMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
            plane.getShader().setBind1f("time", glfwGetTime());
            plane.getShader().setBind1f("Amplitude_min", Amplitude_SumGerstner_min);
            plane.getShader().setBind1f("Amplitude_max", Amplitude_SumGerstner_max);
            plane.getShader().setBind1f("PI", M_PI);
            plane.getShader().setBind1f("g", 9.81); // Accélération de la pesanteur
            plane.getShader().setBind1f("L_min", L_SumGerstner_min);
            plane.getShader().setBind1f("L_max", L_SumGerstner_max);
            plane.getShader().setBind1f("S", S_SumGerstner);
            plane.getShader().setBind1f("Steepness", Steepness_SumGerstner);
            plane.getShader().setBind3f("Direction", Direction_SumGerstner.x, Direction_SumGerstner.y, Direction_SumGerstner.z);
            plane.getShader().setBind1f("Amplitude_FBM", Amplitude_SumGerstner_FBM);
            plane.getShader().setBind1f("Gain_A", Gain_A_SumGerstner);
            plane.getShader().setBind1f("Gain_W", Gain_W_SumGerstner);
            plane.getShader().setBind1f("L_FBM", L_FBM_SumGerstner);
            plane.getShader().setBind3f("lightPosition", LightPos.x, LightPos.y, LightPos.z);
            plane.getShader().setBind3f("viewPosition",cameraPos.x,cameraPos.y,cameraPos.z);
            plane.getShader().setBind3f("lightColor", LightColor.x, LightColor.y, LightColor.z);
            plane.getShader().setBind1f("ambientStrength", ambientStrength);
            plane.getShader().setBind1f("diffuseStrength", diffuseStrength);
            plane.getShader().setBind1f("specularStrength", specularStrength);

            if(materiauSumGerstner == true) {
                plane.getShader().setBind1i("Debug", 0);
            } else if(positionSumGerstner == true) {
                plane.getShader().setBind1i("Debug", 1);
            } else if(uvSumGerstner == true) {
                plane.getShader().setBind1i("Debug", 2);
            } else if(normalSumGerstner == true) {
                plane.getShader().setBind1i("Debug", 3);
            } else if(binormalSumGerstner == true) {
                plane.getShader().setBind1i("Debug", 4);
            } else if(tangentSumGerstner == true) {
                plane.getShader().setBind1i("Debug", 5);
            }

            plane.getShader().setBind1i("nbVagues", numWave_SumGerstner);
            plane.getShader().setBind1i("seed", seed);

            if(FBM_SumGerstner == false) {
                plane.getShader().setBind1i("FBM", 0);
            } else {
                plane.getShader().setBind1i("FBM", 1);
            }


            skybox.useShader();
            skybox.getShader().setBindMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(modelSkybox));
            skybox.getShader().setBindMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
            skybox.getShader().setBindMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
            skybox.getShader().setBind1i("res", resolutionSkybox);
            skybox.bindCubemap(GL_TEXTURE0,0);
            skybox.updateSkybox(GL_TRIANGLES);
            
            // Affichage des objets flottants
            if (objetsApparition){
                manageFlotability->drawSphere(deltaTime,model,view,projection,Amplitude_Sin, L_Sin, S_Sin);
            }


            plane.useShader();
            plane.bindCubemap(GL_TEXTURE1,1);
            plane.updatePlane(GL_TRIANGLES); 
                
            ImGui::Text("Paramètres du shader SumGerstner :");


            if(FBM_SumGerstner == false) {
                ImGui::SliderFloat("Amplitude min", &Amplitude_SumGerstner_min, 0.01f, 2.0f);
                ImGui::SliderFloat("Amplitude max", &Amplitude_SumGerstner_max, 0.01f, 2.0f);
                ImGui::SliderFloat("Wavelength min", &L_SumGerstner_min, 0.01f, 10.0f);
                ImGui::SliderFloat("Wavelength max", &L_SumGerstner_max, 0.01f, 10.0f);
                ImGui::SliderFloat("Steepness", &Steepness_SumGerstner, 0.0f, 1.0f);
                if(ImGui::SliderFloat("Speed", &S_SumGerstner, 0.0f, 20.0f) != 0.) {
                    arretduTemps_SumGerstner = false;
                }
                ImGui::SliderFloat("X", &Direction_SumGerstner.x, -1.0f, 1.0f);
                ImGui::SliderFloat("Z", &Direction_SumGerstner.z, -1.0f, 1.0f);         
            } else {
                ImGui::SliderFloat("Amplitude", &Amplitude_SumGerstner_FBM, 0.01f, 2.0f);
                if(ImGui::SliderFloat("Speed", &S_SumGerstner, 0.0f, 20.0f) != 0.) {
                    arretduTemps_SumGerstner = false;
                }
                ImGui::SliderFloat("Gain amplitude", &Gain_A_SumGerstner, 0.01f, 0.99f);
                ImGui::SliderFloat("Gain wavelength", &Gain_W_SumGerstner,0.01, 5.0f);
                ImGui::SliderFloat("Wavelength", &L_FBM_SumGerstner, 0.01f, 50.0f);
                ImGui::SliderFloat("Steepness", &Steepness_SumGerstner, 0.0f, 0.5f);
            }

            ImGui::SliderInt("Nombre de vagues", &numWave_SumGerstner, 1, 256);
            ImGui::SliderInt("Seed", &seed, 0, 25);
            ImGui::Checkbox("Fractionnal Brownian Motion", &FBM_SumGerstner);


            ImGui::Separator();
            ImGui::Text("Debug du shader");

            if (ImGui::Checkbox("Material", &materiauSumGerstner)) {
                if (materiauSumGerstner) {
                    positionSumGerstner = false;
                    uvSumGerstner = false;
                    normalSumGerstner = false;
                    binormalSumGerstner = false;
                    tangentSumGerstner = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Position", &positionSumGerstner)) {
                if (positionSumGerstner) {
                    materiauSumGerstner = false;
                    uvSumGerstner = false;
                    normalSumGerstner = false;
                    binormalSumGerstner = false;
                    tangentSumGerstner = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("UV", &uvSumGerstner)) {
                if (uvSumGerstner) {
                    materiauSumGerstner = false;
                    positionSumGerstner = false;
                    normalSumGerstner = false;
                    binormalSumGerstner = false;
                    tangentSumGerstner = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Normal", &normalSumGerstner)) {
                if (normalSumGerstner) {
                    materiauSumGerstner = false;
                    positionSumGerstner = false;
                    uvSumGerstner = false;
                    binormalSumGerstner = false;
                    tangentSumGerstner = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Binormale", &binormalSumGerstner)) {
                if (binormalSumGerstner) {
                    materiauSumGerstner = false;
                    positionSumGerstner = false;
                    uvSumGerstner = false;
                    normalSumGerstner = false;
                    tangentSumGerstner = false;
                }
            }
            ImGui::SameLine();
            if (ImGui::Checkbox("Tangente", &tangentSumGerstner)) {
                if (tangentSumGerstner) {
                    materiauSumGerstner = false;
                    positionSumGerstner = false;
                    uvSumGerstner = false;
                    normalSumGerstner = false;
                    binormalSumGerstner = false;
                }
            }

            if (ImGui::Checkbox("Arrêt du temps", &arretduTemps_SumGerstner)) {
                float S_SumGerstner_save = S_SumGerstner;
                if (arretduTemps_SumGerstner) {
                    S_SumGerstner = 0.0;
                } else {
                    S_SumGerstner = 1.0;
                }
            }

            ImGui::Separator();
        }

        if (ModeleFFT) {
            plane.getShader().setBindMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(model));
            plane.getShader().setBindMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
            plane.getShader().setBindMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
            plane.getShader().setBind1f("time", glfwGetTime());

            plane.useShader();

            glActiveTexture(GL_TEXTURE0);
            test.useTexture();
            plane.getShader().setBind1i("tex", 0);

            plane.useComputeShader();
            plane.DispatchWorkGroup(tailleImage, tailleImage, nbThreads, nbThreads);
            plane.getShaderComp().setBind1f("time", glfwGetTime());

            skybox.useShader();
            skybox.getShader().setBindMatrix4fv("model", 1, GL_FALSE, glm::value_ptr(modelSkybox));
            skybox.getShader().setBindMatrix4fv("view", 1, GL_FALSE, glm::value_ptr(view));
            skybox.getShader().setBindMatrix4fv("projection", 1, GL_FALSE, glm::value_ptr(projection));
            skybox.getShader().setBind1i("res", resolutionSkybox);
            skybox.bindCubemap(GL_TEXTURE0,0);
            skybox.updateSkybox(GL_TRIANGLES);
            
            // Affichage des objets flottants
            if (objetsApparition){
                manageFlotability->drawSphere(deltaTime,model,view,projection,Amplitude_Sin, L_Sin, S_Sin);
            }


            plane.useShader();
            plane.updatePlane(GL_TRIANGLES);

            
            ImGui::Separator();
        }

        ImGui::Spacing();

        ImGui::Text("Paramètres du plan :");
        if(ImGui::SliderInt("Résolution", &resolution, 0.0f, 2048.0f)) {
            plane.subdivisediv(resolution);
        }

        ImGui::Spacing();

        if(ImGui::SliderInt("Résolution de la grille", &taillePlan, 1, 500)) {
            plane.updateSize(taillePlan);
        }

        ImGui::Spacing();

        if (ImGui::Checkbox("Fil de fer", &fildefer)) {
            if (fildefer) {
                points = false;
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }
        ImGui::SameLine();
        if (ImGui::Checkbox("Points", &points)) {
            if (points) {
                fildefer = false;
                glPolygonMode(GL_FRONT_AND_BACK, GL_POINT);
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }
        
        ImGui::Spacing();
        
        ImGui::SliderFloat("Coefficient de restitution", manageFlotability->getRefToCoeffRestitution(), 0.5, 3.0);

        if (ImGui::SliderInt("Nombre d'objet", manageFlotability->getRefToNbFlottingObject(), 1, 100)){
            manageFlotability->resetObjets();
            manageFlotability->createBuffer();
            manageFlotability->set_l_pressed(glfwGetTime());
        }

        ImGui::Spacing();

        if (ImGui::SliderFloat("Hauteur d'apparition des objets", &heightSpawn, 1.0, 100.0)){
            manageFlotability->setHeightSpawn(heightSpawn);
            manageFlotability->resetObjets();
            manageFlotability->createBuffer();
            manageFlotability->set_l_pressed(glfwGetTime());
        }

        ImGui::Spacing();

        ImGui::Text("Paramètres de la skybox :");
        ImGui::SliderInt("Résolution de la skybox", &resolutionSkybox, 0, 5000);

        ImGui::Spacing();

        ImGui::SliderInt("Vitesse de la caméra", &speedCam, 10, 500);

        ImGui::End();

        // Light
        glm::vec3 viewPos = cameraPos;

        // Render GUI window
        ImGui::Render();
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData()); 

        glfwSwapBuffers(window.get_window());
        glfwPollEvents();
    }

	delete soundManager;
	
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    window.~Window();
    
    return 0;
}

void processInput(GLFWwindow *window) {
    // Camera sensitivity
    float camera_speed = speedCam * deltaTime;

    if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) glfwSetWindowShouldClose(window,true);
    if (glfwGetKey(window, GLFW_KEY_LEFT) == GLFW_PRESS) horizontal_angle -= camera_speed;
    if (glfwGetKey(window, GLFW_KEY_RIGHT) == GLFW_PRESS) horizontal_angle += camera_speed;
    if (glfwGetKey(window, GLFW_KEY_UP) == GLFW_PRESS) vertical_angle += camera_speed;
    if (glfwGetKey(window, GLFW_KEY_DOWN) == GLFW_PRESS) vertical_angle -= camera_speed;

    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS){
        cameraPos += (camera_speed / 5.f) * cameraTarget;
    }
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS){
        cameraPos -= (camera_speed / 5.f) * cameraTarget;
    }
    if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS){
        cameraPos += (camera_speed / 5.f) * cameraUp;
    }
    if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS){
        cameraPos -= (camera_speed / 5.f) * cameraUp;
    }
    if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS){
        cameraPos += (camera_speed / 5.f) * glm::normalize(glm::cross(cameraTarget,cameraUp));;
    }
    if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS){
        cameraPos -= (camera_speed / 5.f) * glm::normalize(glm::cross(cameraTarget,cameraUp));;
    }

    // Pour sortir de la caméra controlée à la souris
    if (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS){ 
        cameraMouse = false;
    }
    if (glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS){ 
        cameraMouse = true;
    }
    
    // Fais appaitre les objets qui vont flotter
    if (glfwGetKey(window, GLFW_KEY_L) == GLFW_PRESS){ 
        objetsApparition = true;
        manageFlotability->resetObjets();
        manageFlotability->set_l_pressed(glfwGetTime());
    }

    //std::cout << cameraPos[0] << "\t" << cameraPos[1] << "\t" << cameraPos[2] << std::endl;

    // Ajuster le zoom
    if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) radius -= 10*camera_speed; // 10 fois pour que ça aille plus vite sans modifier la vitesse de l'orbite
    if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) radius += 10*camera_speed;

    // Limiter l'angle vertical
    vertical_angle = std::max(-1.57f, std::min(1.57f, vertical_angle));
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    // Adjust the viewport
    glViewport(0, 0, width, height);
}