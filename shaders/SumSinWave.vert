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
out vec3 positionWorld;
out vec3 normalWorld;

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

vec3 Add_Wave(vec3 Pos, float S, vec3 Direction, float time) {
    vec3 newPos = Pos;
    
    float wave = 0.0f;
    float L;
    float w;

    float waveInit = 0.0f;
    float wFBM = (2.0 * PI) / L_FBM;
    float AFBM = Amplitude_FBM;
    float dfdx;
    vec3 modifiedPos = aPos;

    if (FBM == 0) {
        for (int i = 0; i < nbVagues; i++) {
            L = mix(L_min, L_max, rand(vec2(float(i), seed))); 
            w = (2.0 * PI) / L;

            float randomAmplitude = mix(Amplitude_min, Amplitude_max, rand(vec2(float(i), seed)));
            float randomPhase = rand(vec2(float(i) + 0.1, seed)) * 2.0 * PI;
            vec3 randomDirection = normalize(reflect(Direction, normalize(vec3(rand(vec2(float(i) + 0.2, seed)), 
                                                                            rand(vec2(float(i) + 0.3, seed)), 
                                                                            rand(vec2(float(i) + 0.4, seed))))));

            float phi_t = (S * w) * time + randomPhase;
            wave += randomAmplitude * sin(dot(randomDirection, aPos.xyz) * w + phi_t);
        }
    } else if (FBM == 1) {
        for (int i = 0; i < nbVagues; i++) {
            vec3 randomDirection = randomDir(float(i) + seed);
            float phi_t = (S * wFBM) * time;

            if (DW == 0) {
                wave += AFBM * sin(dot(randomDirection, aPos.xyz) * wFBM + phi_t);
            } else {
                waveInit = AFBM * sin(dot(randomDirection, aPos.xyz) * wFBM + phi_t);
                wave += AFBM * sin(dot(randomDirection, modifiedPos.xyz) * wFBM + phi_t);

                dfdx = AFBM * wFBM * randomDirection.x * cos(dot(randomDirection.xyz, aPos.xyz) * wFBM + phi_t);
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

    float wave = 0.0f;
    float L;
    float w;

    float waveInit = 0.0f;
    float wFBM = (2.0 * PI) / L_FBM;
    float AFBM = Amplitude_FBM;
    float dfdz = 0.0;
    vec3 modifiedPos = aPos;
    vec2 binormale;

    if (FBM == 0) {
        for (int i = 0; i < nbVagues; i++) {
            L = mix(L_min, L_max, rand(vec2(float(i), seed))); 
            w = (2.0 * PI) / L;

            float randomAmplitude = mix(Amplitude_min, Amplitude_max, rand(vec2(float(i), seed)));
            float randomPhase = rand(vec2(float(i) + 0.1, seed)) * 2.0 * PI;
            vec3 randomDirection = normalize(reflect(Direction, normalize(vec3(rand(vec2(float(i) + 0.2, seed)), 
                                                                            rand(vec2(float(i) + 0.3, seed)), 
                                                                            rand(vec2(float(i) + 0.4, seed))))));

            float phi_t = (S * w) * time + randomPhase;
            dfdz += randomDirection.z * cos(dot(randomDirection.xz, aPos.xz));
        }
    } else if (FBM == 1) {
        for (int i = 0; i < nbVagues; i++) {
            vec3 randomDirection = randomDir(float(i) + seed);
            float phi_t = (S * wFBM) * time;

            if (DW == 0) {
                dfdz += randomDirection.z * cos(dot(randomDirection.xz, aPos.xz));
            } else {
                dfdz += randomDirection.z * cos(dot(randomDirection.xz, aPos.xz));
            }

            wFBM *= Gain_W;
            AFBM *= Gain_A;
        }        
    }

    return normalize(vec3(0.0, dfdz, 1.0));
}

vec3 ComputeNormale(vec3 Pos, float S, vec3 Direction, float time) {
    vec3 newPos = Pos;

    float wave = 0.0f;
    float L;
    float w;

    float waveInit = 0.0f;
    float wFBM = (2.0 * PI) / L_FBM;
    float AFBM = Amplitude_FBM;
    float dx = 0;
    float dz = 0;
    vec3 modifiedPos = aPos;
    vec2 normal = vec2(0.0, 0.0);

    if (FBM == 0) {
        for (int i = 0; i < nbVagues; i++) {
            L = mix(L_min, L_max, rand(vec2(float(i), seed))); 
            w = (2.0 * PI) / L;

            float randomAmplitude = mix(Amplitude_min, Amplitude_max, rand(vec2(float(i), seed)));
            float randomPhase = rand(vec2(float(i) + 0.1, seed)) * 2.0 * PI;
            vec3 randomDirection = normalize(reflect(Direction, normalize(vec3(rand(vec2(float(i) + 0.2, seed)), 
                                                                            rand(vec2(float(i) + 0.3, seed)), 
                                                                            rand(vec2(float(i) + 0.4, seed))))));

            float phi_t = (S * w) * time + randomPhase;
            wave += randomAmplitude * sin(dot(randomDirection, aPos.xyz) * w + phi_t);
            normal += w * randomAmplitude * randomDirection.xz * cos(dot(randomDirection.xz, aPos.xz) * w + phi_t);
            dx += randomDirection.x * cos(dot(randomDirection.xz, aPos.xz) * w + time);
            dz += randomDirection.z * cos(dot(randomDirection.xz, aPos.xz) * w + time);
        }
    } else if (FBM == 1) {
        for (int i = 0; i < nbVagues; i++) {
            vec3 randomDirection = randomDir(float(i) + seed);
            float phi_t = (S * wFBM) * time;

            if (DW == 0) {
                normal += wFBM * AFBM * randomDirection.xz * cos(dot(randomDirection.xz, aPos.xz) * wFBM + phi_t); 
                dx += randomDirection.x * cos(dot(randomDirection.xz, aPos.xz) * wFBM + time);
                dz += randomDirection.z * cos(dot(randomDirection.xz, aPos.xz) * wFBM + time);
            } else {
                normal += wFBM * AFBM * randomDirection.xz * cos(dot(randomDirection.xz, aPos.xz) * wFBM + phi_t);
                dx += randomDirection.x * cos(dot(randomDirection.xz, aPos.xz) * wFBM + time);
                dz += randomDirection.z * cos(dot(randomDirection.xz, aPos.xz) * wFBM + time);
            }

            wFBM *= Gain_W;
            AFBM *= Gain_A;
        }        
    }

    vec3 normalTest = vec3(dx, 1.0, dz);
    return normalize(normalTest);
}

vec3 ComputeTangente(vec3 Pos, float S, vec3 Direction, float time) {
    vec3 newPos = Pos; 

    float wave = 0.0;
    float L;
    float w;

    float waveInit = 0.0;
    float wFBM = (2.0 * PI) / L_FBM;
    float AFBM = Amplitude_FBM;
    float dfdx = 0.0;
    vec3 modifiedPos = aPos;

    if (FBM == 0) {
        for (int i = 0; i < nbVagues; i++) {
            L = mix(L_min, L_max, rand(vec2(float(i), seed))); 
            w = (2.0 * PI) / L;

            float randomAmplitude = mix(Amplitude_min, Amplitude_max, rand(vec2(float(i), seed)));
            float randomPhase = rand(vec2(float(i) + 0.1, seed)) * 2.0 * PI;
            vec3 randomDirection = normalize(reflect(Direction, normalize(vec3(rand(vec2(float(i) + 0.2, seed)), 
                                                                            rand(vec2(float(i) + 0.3, seed)), 
                                                                            rand(vec2(float(i) + 0.4, seed))))));

            float phi_t = (S * w) * time + randomPhase;
            dfdx += randomDirection.x * cos(dot(randomDirection.xz, aPos.xz));
        }
    } else if (FBM == 1) {
        for (int i = 0; i < nbVagues; i++) {
            vec3 randomDirection = randomDir(float(i) + seed);
            float phi_t = (S * wFBM) * time;

            if (DW == 0) {
                dfdx += randomDirection.x * cos(dot(randomDirection.xz, aPos.xz));
            } else {
                dfdx += randomDirection.x * cos(dot(randomDirection.xz, aPos.xz));
                modifiedPos += vec3(dfdx, 0.0, 0.0);
            }

            wFBM *= Gain_W;
            AFBM *= Gain_A;
        }        
    }
    return normalize(vec3(1.0f, dfdx, 0.0f));
}

void main() {
    vec3 accPos = vec3(0.0);
    vec3 accBin = vec3(0.0);
    vec3 accTan = vec3(0.0);
    vec3 accNor = vec3(0.0);

    accPos = Add_Wave(aPos, S, Direction, time);
    accBin = ComputeBinormale(aPos, S, Direction, time);
    accTan = ComputeTangente(aPos, S, Direction, time);
    accNor = ComputeNormale(aPos, S, Direction, time);
    //accNor = cross(accTan, accBin);

    vec3 newWave = accPos;
    gl_Position = projection * view * model * vec4(newWave, 1.0f);

    binormale = accBin;
    tangente = accTan;
    normal = accNor;
    height = newWave.y;
    pos = newWave;
    normalWorld = normalize(mat3(transpose(inverse(model))) * normal); 
    positionWorld = (model * vec4(newWave, 1.0f)).xyz;
    tex = aTex;
}