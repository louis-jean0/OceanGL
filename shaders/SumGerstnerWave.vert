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
    float angleX = rand(vec2(seed, 0.0)) * 2. * PI;
    float angleY = rand(vec2(seed, 1.0)) * 2. * PI;
    float angleZ = rand(vec2(seed, 2.0)) * 2. * PI;

    float x = cos(angleX); 
    float y = cos(angleY); 
    float z = cos(angleZ); 

    vec3 dir = normalize(vec3(x, y, z));

    return dir;
}

// Fonction pour une vague basée sur le modèle de Gerstner
vec3 Add_Wave(vec3 Pos, float S, vec3 Direction, float time) {
    vec3 newPos = aPos;
    vec3 newWave = vec3(0.0f);
    vec3 finalWave = vec3(0.0f);

    float w;  
    float Q;
    float L;

    // Avec FBM
    // Gain_A < Gain_W obligatoirement
    float wFBM = sqrt(g * ((2. * PI) / L_FBM));
    float AFBM = Amplitude_FBM;
    vec3 dfdx;
    vec3 modifiedPos = aPos;


    if(FBM == 0) {
        for(int i = 0; i < nbVagues; i++) {
            L = mix(L_min, L_max, rand(vec2(float(i), seed))); 

            float randomAmplitude = mix(Amplitude_min, Amplitude_max, rand(vec2(float(i), seed)));
            float randomPhase = rand(vec2(float(i) + 0.1, seed)) * 2.0 * PI;
            vec3 randomDirection = normalize(reflect(Direction, normalize(vec3(rand(vec2(float(i) + 0.2, seed)), 
                                                                                rand(vec2(float(i) + 0.3, seed)), 
                                                                                rand(vec2(float(i) + 0.4, seed))))));
            // Si Steepness < 0.5, pic + doux sinon pic très abrupte 
            float randompeak = mix(Steepness, 0.5f, rand(vec2(float(i), seed)));

            w = sqrt(g * ((2. * PI) / L));
            Q = randompeak / (w * randomAmplitude * nbVagues);
            float phi_t = (S * (2. / L)) * time + randomPhase;;

            // Fonction principale
            newWave.x += ((Q * randomAmplitude) * randomDirection.x * cos((w * dot(randomDirection.xyz, aPos.xyz)) + phi_t));
            newWave.z += ((Q * randomAmplitude) * randomDirection.z * cos((w * dot(randomDirection.xyz, aPos.xyz)) + phi_t));
            newWave.y = randomAmplitude * sin(w * dot(randomDirection.xyz, aPos.xyz) + phi_t);

            finalWave += newWave;
        }
    } else if(FBM == 1) {
        for(int i = 0; i < nbVagues; i++) {
            vec3 randomDirection = randomDir(float(i) + seed);
            Q = Steepness / (wFBM * AFBM * nbVagues);
            float phi_t = (S * (2. / L_FBM)) * time;

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
    vec3 binormal = Pos;
    vec3 newWave = vec3(0.0f);
    vec3 finalWave = vec3(0.0f);

    float w;  
    float Q;
    float L;

    // Avec FBM
    // Gain_A < Gain_W obligatoirement
    float wFBM = sqrt(g * ((2. * PI) / L_FBM));
    float AFBM = Amplitude_FBM;
    vec3 dfdx;
    vec3 modifiedPos = aPos;


    if(FBM == 0) {
        for(int i = 0; i < nbVagues; i++) {
            L = mix(L_min, L_max, rand(vec2(float(i), seed))); 

            float randomAmplitude = mix(Amplitude_min, Amplitude_max, rand(vec2(float(i), seed)));
            float randomPhase = rand(vec2(float(i) + 0.1, seed)) * 2.0 * PI;
            vec3 randomDirection = normalize(reflect(Direction, normalize(vec3(rand(vec2(float(i) + 0.2, seed)), 
                                                                                rand(vec2(float(i) + 0.3, seed)), 
                                                                                rand(vec2(float(i) + 0.4, seed))))));
            // Si Steepness < 0.5, pic + doux sinon pic très abrupte 
            float randompeak = mix(Steepness, 0.5f, rand(vec2(float(i), seed)));

            w = sqrt(g * ((2. * PI) / L));
            Q = randompeak / (w * randomAmplitude * nbVagues);
            float phi_t = (S * (2. / L)) * time + randomPhase;

            float WA = w * randomAmplitude;
            float SA = sin(w * dot(randomDirection.xyz, aPos.xyz) + phi_t);
            float CA = cos(w * dot(randomDirection.xyz, aPos.xyz) + phi_t);

            newWave.x = 1.0 - (Q * (randomDirection.x * randomDirection.x) * WA * SA);
            newWave.z = 0.0 - (Q * (randomDirection.x * randomDirection.z) * WA * SA);
            newWave.y = Direction.x * WA * CA;

            finalWave += newWave;
        }
    } else if(FBM == 1) {
        for(int i = 0; i < nbVagues; i++) {
            vec3 randomDirection = randomDir(float(i) + seed);
            Q = Steepness / (wFBM * AFBM * nbVagues);
            float phi_t = (S * (2. / L_FBM)) * time;

            float WA = wFBM * AFBM;
            float SA = sin(wFBM * dot(randomDirection.xyz, aPos.xyz) + phi_t);
            float CA = cos(wFBM * dot(randomDirection.xyz, aPos.xyz) + phi_t);

            newWave.x = 1.0 - (Q * (randomDirection.x * randomDirection.x) * WA * SA);
            newWave.z = 0.0 - (Q * (randomDirection.x * randomDirection.z) * WA * SA);
            newWave.y = Direction.x * WA * CA;

            finalWave += newWave;

            wFBM *= Gain_W;
            AFBM *= Gain_A;
        }        
    }

    binormal = finalWave;

    return normalize(binormal);
}

vec3 ComputeTangente(vec3 Pos, float S, vec3 Direction, float time) {
    vec3 tangent = Pos;
    vec3 newWave = vec3(0.0f);
    vec3 finalWave = vec3(0.0f);

    float w;  
    float Q;
    float L;

    // Avec FBM
    // Gain_A < Gain_W obligatoirement
    float wFBM = sqrt(g * ((2. * PI) / L_FBM));
    float AFBM = Amplitude_FBM;
    vec3 dfdx;
    vec3 modifiedPos = aPos;


    if(FBM == 0) {
        for(int i = 0; i < nbVagues; i++) {
            L = mix(L_min, L_max, rand(vec2(float(i), seed))); 

            float randomAmplitude = mix(Amplitude_min, Amplitude_max, rand(vec2(float(i), seed)));
            float randomPhase = rand(vec2(float(i) + 0.1, seed)) * 2.0 * PI;
            vec3 randomDirection = normalize(reflect(Direction, normalize(vec3(rand(vec2(float(i) + 0.2, seed)), 
                                                                                rand(vec2(float(i) + 0.3, seed)), 
                                                                                rand(vec2(float(i) + 0.4, seed))))));
            // Si Steepness < 0.5, pic + doux sinon pic très abrupte 
            float randompeak = mix(Steepness, 0.5f, rand(vec2(float(i), seed)));

            w = sqrt(g * ((2. * PI) / L));
            Q = randompeak / (w * randomAmplitude * nbVagues);
            float phi_t = (S * (2. / L)) * time + randomPhase;

            float WA = w * randomAmplitude;
            float SA = sin(w * dot(randomDirection.xyz, aPos.xyz) + phi_t);
            float CA = cos(w * dot(randomDirection.xyz, aPos.xyz) + phi_t);

            newWave.x = 0. - (Q * (randomDirection.x * randomDirection.z) * WA * SA);
            newWave.z = 1. - (Q * (randomDirection.z * randomDirection.z) * WA * SA);
            newWave.y = randomDirection.z * WA * CA;

            finalWave += newWave;
        }
    } else if(FBM == 1) {
        for(int i = 0; i < nbVagues; i++) {
            vec3 randomDirection = randomDir(float(i) + seed);
            Q = Steepness / (wFBM * AFBM * nbVagues);
            float phi_t = (S * (2. / L_FBM)) * time;

            float WA = wFBM * AFBM;
            float SA = sin(wFBM * dot(randomDirection.xyz, aPos.xyz) + phi_t);
            float CA = cos(wFBM * dot(randomDirection.xyz, aPos.xyz) + phi_t);

            newWave.x = 0. - (Q * (randomDirection.x * randomDirection.z) * WA * SA);
            newWave.z = 1. - (Q * (randomDirection.z * randomDirection.z) * WA * SA);
            newWave.y = randomDirection.z * WA * CA;

            finalWave += newWave;

            wFBM *= Gain_W;
            AFBM *= Gain_A;
        }        
    }

    tangent = finalWave;

    return normalize(tangent);
}

vec3 ComputeNormal(vec3 Pos, float S, vec3 Direction, float time) {
    vec3 normal = Pos;
    vec3 newWave = vec3(0.0f);
    vec3 finalWave = vec3(0.0f);

    float w;  
    float Q;
    float L;

    // Avec FBM
    // Gain_A < Gain_W obligatoirement
    float wFBM = sqrt(g * ((2. * PI) / L_FBM));
    float AFBM = Amplitude_FBM;
    vec3 dfdx;
    vec3 modifiedPos = aPos;


    if(FBM == 0) {
        for(int i = 0; i < nbVagues; i++) {
            L = mix(L_min, L_max, rand(vec2(float(i), seed))); 

            float randomAmplitude = mix(Amplitude_min, Amplitude_max, rand(vec2(float(i), seed)));
            float randomPhase = rand(vec2(float(i) + 0.1, seed)) * 2.0 * PI;
            vec3 randomDirection = normalize(reflect(Direction, normalize(vec3(rand(vec2(float(i) + 0.2, seed)), 
                                                                                rand(vec2(float(i) + 0.3, seed)), 
                                                                                rand(vec2(float(i) + 0.4, seed))))));
            // Si Steepness < 0.5, pic + doux sinon pic très abrupte 
            float randompeak = mix(Steepness, 0.5f, rand(vec2(float(i), seed)));

            w = sqrt(g * ((2. * PI) / L));
            Q = randompeak / (w * randomAmplitude * nbVagues);
            float phi_t = (S * (2. / L)) * time + randomPhase;

            float WA = w * randomAmplitude;
            float SA = sin(w * dot(randomDirection.xyz, aPos.xyz) + phi_t);
            float CA = cos(w * dot(randomDirection.xyz, aPos.xyz) + phi_t);

            newWave.x = 0. - (randomDirection.x * WA * CA);
            newWave.z = 0. - (randomDirection.z * WA * CA);
            newWave.y = 1. - (Q * WA * SA);

            finalWave += newWave;
        }
    } else if(FBM == 1) {
        for(int i = 0; i < nbVagues; i++) {
            vec3 randomDirection = randomDir(float(i) + seed);
            Q = Steepness / (wFBM * AFBM * nbVagues);
            float phi_t = (S * (2. / L_FBM)) * time;

            float WA = wFBM * AFBM;
            float SA = sin(wFBM * dot(randomDirection.xyz, aPos.xyz) + phi_t);
            float CA = cos(wFBM * dot(randomDirection.xyz, aPos.xyz) + phi_t);

            newWave.x = 0. - (randomDirection.x * WA * CA);
            newWave.z = 0. - (randomDirection.z * WA * CA);
            newWave.y = 1. - (Q * WA * SA);

            finalWave += newWave;

            wFBM *= Gain_W;
            AFBM *= Gain_A;
        }        
    }

    normal = finalWave;

    return normalize(normal);
}


void main()
{
    vec3 accPos = vec3(0.0);
    vec3 accBin = vec3(0.0);
    vec3 accTan = vec3(0.0);
    vec3 accNor = vec3(0.0);


    accPos = Add_Wave(aPos, S, Direction, time);
    accBin = ComputeBinormale(aPos, S, Direction, time);
    accTan = ComputeTangente(aPos, S, Direction, time);
    accNor = ComputeNormal(aPos, S, Direction, time);
    
    //accNor = cross(accTan, accBin);

    vec3 newWave = accPos;
    gl_Position = projection * view * model * vec4(newWave, 1.0f);

    binormale = accBin;
    tangente = accTan;
    normal = accNor;
    height = newWave.y;
    pos = newWave;
    tex = aTex;
}