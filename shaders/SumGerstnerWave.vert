#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in vec3 aNormal;

// Colors vertices
out float height;
out vec3 pos;
out vec3 normal;
out vec2 tex;
out vec3 binormale;
out vec3 tangente;

// General variables
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

uniform float PI;
uniform float Amplitude_min;
uniform float Amplitude_max;
uniform float Amplitude_FBM;

uniform float g;
uniform float L_min;
uniform float L_max;
uniform float Steepness;
uniform float S;
uniform vec3 Direction;

uniform int nbVagues;
uniform int seed;
uniform int FBM;
uniform float Gain_A;
uniform float Gain_W;
uniform float L_FBM;

float rand(vec2 co) {
    return fract(sin(dot(co.xy, vec2(12.9898, 78.233))) * 43758.5453);
}

vec3 randomDir(float seed) {
    float angle = rand(vec2(seed, 0.0)) * 2.0 * PI;
    float x = cos(angle);
    float y = sin(angle);
    return vec3(x, 0.0, y);
}

// Fonction pour une vague basée sur le modèle de Gerstner
vec3 Add_Wave(vec3 Pos, float S, vec3 Direction, float time) {
    vec3 newPos = aPos;
    vec3 newWave = vec3(0.0f);
    vec3 finalWave = vec3(0.0f);

    float w;  
    float Q;
    float L;

    float wFBM = sqrt(g * ((2.0 * PI) / L_FBM));
    float AFBM = Amplitude_FBM;

    if(FBM == 0) {
        for(int i = 0; i < nbVagues; i++) {
            L = mix(L_min, L_max, rand(vec2(float(i), seed))); 
            float randomAmplitude = mix(Amplitude_min, Amplitude_max, rand(vec2(float(i), seed)));
            float randomPhase = rand(vec2(float(i) + 0.1, seed)) * 2.0 * PI;
            vec3 randomDirection = normalize(reflect(Direction, normalize(vec3(rand(vec2(float(i) + 0.2, seed)), 
                                                                               rand(vec2(float(i) + 0.3, seed)), 
                                                                               rand(vec2(float(i) + 0.4, seed))))));
            float randompeak = mix(Steepness, 0.5f, rand(vec2(float(i), seed)));
            w = sqrt(g * ((2.0 * PI) / L));
            Q = randompeak / (w * randomAmplitude * nbVagues);
            float phi_t = (S * (2.0 / L)) * time + randomPhase;
            newWave.x += ((Q * randomAmplitude) * randomDirection.x * cos((w * dot(randomDirection.xyz, aPos.xyz)) + phi_t));
            newWave.z += ((Q * randomAmplitude) * randomDirection.z * cos((w * dot(randomDirection.xyz, aPos.xyz)) + phi_t));
            newWave.y = randomAmplitude * sin(w * dot(randomDirection.xyz, aPos.xyz) + phi_t);
            finalWave += newWave;
        }
    } else if(FBM == 1) {
        for(int i = 0; i < nbVagues; i++) {
            vec3 randomDirection = randomDir(float(i) + seed);
            Q = Steepness / (wFBM * AFBM * nbVagues);
            float phi_t = (S * (2.0 / L_FBM)) * time;
            newWave.x += ((Q * AFBM) * randomDirection.x * cos((wFBM * dot(randomDirection.xyz, aPos.xyz)) + phi_t));
            newWave.z += ((Q * AFBM) * randomDirection.z * cos((wFBM * dot(randomDirection.xyz, aPos.xyz)) + phi_t));
            newWave.y = AFBM * sin(wFBM * dot(randomDirection.xyz, aPos.xyz) + phi_t);
            finalWave += newWave;
            wFBM *= Gain_W;
            AFBM *= Gain_A;
        }        
    }

    newPos += finalWave;   
    return newPos;
}

vec3 ComputeBinormale(vec3 Pos, float S, vec3 Direction, float time) {
    vec3 binormal = vec3(1.0, 0.0, 0.0);
    vec3 finalWave = vec3(0.0f);
    float wFBM = sqrt(g * ((2.0 * PI) / L_FBM));
    float AFBM = Amplitude_FBM;

    for(int i = 0; i < nbVagues; i++) {
        vec3 randomDirection = randomDir(float(i) + seed);
        float phi_t = (S * (2.0 / L_FBM)) * time;
        float WA = wFBM * AFBM;
        float SA = sin(wFBM * dot(randomDirection.xyz, Pos.xyz) + phi_t);
        binormal.x -= randomDirection.x * WA * SA;
        binormal.z -= randomDirection.z * WA * SA;
        binormal.y = 1.0 - Steepness * WA * SA;
        wFBM *= Gain_W;
        AFBM *= Gain_A;
    }

    return normalize(binormal);
}

vec3 ComputeTangente(vec3 Pos, float S, vec3 Direction, float time) {
    vec3 tangent = vec3(0.0, 0.0, 1.0);
    vec3 finalWave = vec3(0.0f);
    float wFBM = sqrt(g * ((2.0 * PI) / L_FBM));
    float AFBM = Amplitude_FBM;

    for(int i = 0; i < nbVagues; i++) {
        vec3 randomDirection = randomDir(float(i) + seed);
        float phi_t = (S * (2.0 / L_FBM)) * time;
        float WA = wFBM * AFBM;
        float SA = sin(wFBM * dot(randomDirection.xyz, Pos.xyz) + phi_t);
        tangent.x -= randomDirection.x * WA * SA;
        tangent.z -= randomDirection.z * WA * SA;
        tangent.y = 1.0 - Steepness * WA * SA;
        wFBM *= Gain_W;
        AFBM *= Gain_A;
    }

    return normalize(tangent);
}

vec3 ComputeNormal(vec3 Pos, float S, vec3 Direction, float time) {
    vec3 normal = vec3(0.0, 1.0, 0.0);
    vec3 finalWave = vec3(0.0f);
    float wFBM = sqrt(g * ((2.0 * PI) / L_FBM));
    float AFBM = Amplitude_FBM;

    for(int i = 0; i < nbVagues; i++) {
        vec3 randomDirection = randomDir(float(i) + seed);
        float phi_t = (S * (2.0 / L_FBM)) * time;
        float WA = wFBM * AFBM;
        float CA = cos(wFBM * dot(randomDirection.xyz, Pos.xyz) + phi_t);
        normal.x -= randomDirection.x * WA * CA;
        normal.z -= randomDirection.z * WA * CA;
        normal.y = 1.0 - Steepness * WA * CA;
        wFBM *= Gain_W;
        AFBM *= Gain_A;
    }

    return normalize(normal);
}

void main() {
    vec3 accPos = Add_Wave(aPos, S, Direction, time);
    vec3 accBin = ComputeBinormale(aPos, S, Direction, time);
    vec3 accTan = ComputeTangente(aPos, S, Direction, time);
    vec3 accNor = ComputeNormal(aPos, S, Direction, time);

    vec3 newWave = accPos;
    gl_Position = projection * view * model * vec4(newWave, 1.0f);

    binormale = accBin;
    tangente = accTan;
    normal = accNor;
    height = newWave.y;
    pos = newWave;
    tex = aTex;
}