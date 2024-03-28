#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in vec3 aNormal;

// Variables de sorties
out float height;
out vec3 pos;
out vec3 normal;
out vec2 tex;
out vec3 binormale;
out vec3 tangente;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

uniform float Amplitude_min;
uniform float Amplitude_max;
uniform float Amplitude_FBM;
uniform float L_min;
uniform float L_max;
uniform float S;
uniform vec3 Direction;
uniform float PI;
uniform float Gain_A;
uniform float Gain_W;
uniform float L_FBM;

uniform int nbVagues;
uniform int seed;
uniform int FBM;
uniform int DW;


float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 randomDir(float seed) {
    float angle = rand(vec2(seed, 0.0)) * 2.0 * PI;
    
    float x = cos(angle);
    float y = sin(angle);
    
    return vec3(x, 0.0, y);
}

// Fonction pour une vague sinusoïdale
vec3 Add_Wave(vec3 Pos, float S, vec3 Direction, float time) {
    vec3 newPos = Pos;
    
    // Sans FBM
    float wave = 0.0f;
    float L;
    float w;

    // Avec FBM
    // Gain_A < Gain_W obligatoirement
    float wFBM = (2.0 * PI) / L_FBM;
    float AFBM = Amplitude_FBM;
    float dfdx;
    vec3 modifiedPos = aPos;

    if(FBM == 0) {
        for(int i = 0; i < nbVagues; i++) {
            L = mix(L_min, L_max, rand(vec2(float(i), seed)));
            w = (2.0 * PI) / L;

            float randomAmplitude = mix(Amplitude_min, Amplitude_max, rand(vec2(float(i), seed)));
            float randomPhase = rand(vec2(float(i) + 0.1, seed)) * 2.0 * PI;
            vec3 randomDirection = normalize(reflect(Direction, normalize(vec3(rand(vec2(float(i) + 0.2, seed)), 
                                                                            rand(vec2(float(i) + 0.3, seed)), 
                                                                            rand(vec2(float(i) + 0.4, seed))))));

            float phi_t = (S * w) * time + randomPhase;
            wave += randomAmplitude * sin(dot(randomDirection.xz, aPos.xz) * w + phi_t);
        }
    } else if(FBM == 1) {
        for(int i = 0; i < nbVagues; i++) {
            vec3 randomDirection = randomDir(float(i) + seed);
            float phi_t = (S * wFBM) * time;

            if(DW == 0) {
                wave += AFBM * sin(dot(randomDirection.xz, aPos.xz) * wFBM + phi_t);
            } else {

                wave += AFBM * sin(dot(randomDirection.xz, modifiedPos.xz) * wFBM + phi_t);

                // Domain Warping --> effet qui fait en sorte que les vagues se repoussent
                dfdx = AFBM * wFBM * randomDirection.x * cos(dot(randomDirection.xz, aPos.xz) * wFBM + phi_t);
                modifiedPos += vec3(dfdx, 0.0, 0.0);
            }

            wFBM *= Gain_W;
            AFBM *= Gain_A;
        }        
    }

    newPos.y += wave;   
       
    return newPos;
}

vec3 ComputeBinormale(vec3 Pos, float S, vec3 Direction, float time) {
    vec3 newPos = Pos; 

    // Sans FBM
    float wave = 0.0f;
    float L;
    float w;

    // Avec FBM
    // Gain_A < Gain_W obligatoirement
    float wFBM = (2.0 * PI) / L_FBM;
    float AFBM = Amplitude_FBM;
    float dfdx;
    vec3 modifiedPos = aPos;

    if(FBM == 0) {
        for(int i = 0; i < nbVagues; i++) {
            L = mix(L_min, L_max, rand(vec2(float(i), seed))); 
            w = (2.0 * PI) / L;

            float randomAmplitude = mix(Amplitude_min, Amplitude_max, rand(vec2(float(i), seed)));
            float randomPhase = rand(vec2(float(i) + 0.1, seed)) * 2.0 * PI;
            vec3 randomDirection = normalize(reflect(Direction, normalize(vec3(rand(vec2(float(i) + 0.2, seed)), 
                                                                            rand(vec2(float(i) + 0.3, seed)), 
                                                                            rand(vec2(float(i) + 0.4, seed))))));

            float phi_t = (S * w) * time + randomPhase;
            wave += randomAmplitude * Direction.x * w * cos(dot(randomDirection.xz, aPos.xz) * w + phi_t);
        }
    } else if(FBM == 1) {
        for(int i = 0; i < nbVagues; i++) {
            vec3 randomDirection = randomDir(float(i) + seed);
            float phi_t = (S * wFBM) * time;

            if(DW == 0) {
                wave += AFBM * Direction.x * wFBM * cos(dot(randomDirection.xz, aPos.xz) * wFBM + phi_t);
            } else {

                wave += AFBM * Direction.x * wFBM * cos(dot(randomDirection.xz, modifiedPos.xz) * wFBM + phi_t);

                // Domain Warping --> effet qui fait en sorte que les vagues se repoussent
                dfdx = AFBM * wFBM * randomDirection.x * cos(dot(randomDirection.xz, aPos.xz) * wFBM + phi_t);
                modifiedPos += vec3(dfdx, 0.0, 0.0);
            }

            wFBM *= Gain_W;
            AFBM *= Gain_A;
        }        
    }

    newPos.y += wave;   

    return normalize(vec3(1.0f, newPos.y, 0.f));
}

vec3 ComputeTangente(vec3 Pos, float S, vec3 Direction, float time) {
    vec3 newPos = Pos; 

    // Sans FBM
    float wave = 0.0f;
    float L;
    float w;

    // Avec FBM
    // Gain_A < Gain_W obligatoirement
    float wFBM = (2.0 * PI) / L_FBM;
    float AFBM = Amplitude_FBM;
    float dfdx;
    vec3 modifiedPos = aPos;

    if(FBM == 0) {
        for(int i = 0; i < nbVagues; i++) {
            L = mix(L_min, L_max, rand(vec2(float(i), seed))); 
            w = (2.0 * PI) / L;

            float randomAmplitude = mix(Amplitude_min, Amplitude_max, rand(vec2(float(i), seed)));
            float randomPhase = rand(vec2(float(i) + 0.1, seed)) * 2.0 * PI;
            vec3 randomDirection = normalize(reflect(Direction, normalize(vec3(rand(vec2(float(i) + 0.2, seed)), 
                                                                            rand(vec2(float(i) + 0.3, seed)), 
                                                                            rand(vec2(float(i) + 0.4, seed))))));

            float phi_t = (S * w) * time + randomPhase;
            wave += randomAmplitude * Direction.z * w * cos(dot(randomDirection.xz, aPos.xz) * w + phi_t);
        }
    } else if(FBM == 1) {
        for(int i = 0; i < nbVagues; i++) {
            vec3 randomDirection = randomDir(float(i) + seed);
            float phi_t = (S * wFBM) * time;

            if(DW == 0) {
                wave += AFBM * Direction.z * wFBM * cos(dot(randomDirection.xz, aPos.xz) * wFBM + phi_t);
            } else {

                wave += AFBM * Direction.z * wFBM * cos(dot(randomDirection.xz, modifiedPos.xz) * wFBM + phi_t);

                // Domain Warping --> effet qui fait en sorte que les vagues se repoussent
                dfdx = AFBM * wFBM * randomDirection.x * cos(dot(randomDirection.xz, aPos.xz) * wFBM + phi_t);
                modifiedPos += vec3(dfdx, 0.0, 0.0);
            }

            wFBM *= Gain_W;
            AFBM *= Gain_A;
        }        
    }

    newPos.y += wave;  

    return normalize(vec3(0.0f, newPos.y, 1.0f));
}

void main() {
    vec3 accPos = vec3(0.0);
    vec3 accBin = vec3(0.0);
    vec3 accTan = vec3(0.0);
    vec3 accNor = vec3(0.0);
  
    accPos = Add_Wave(aPos, S, Direction, time);
    accBin = ComputeBinormale(aPos, S, Direction, time);
    accTan = ComputeTangente(aPos, S, Direction, time);
    accNor = cross(accTan, accBin);

    vec3 newWave = accPos;
    gl_Position = projection * view * model * vec4(newWave, 1.0f);

    binormale = accBin;
    tangente = accTan;
    normal = accNor;
    height = newWave.y;
    pos = newWave;
    tex = aTex;
}