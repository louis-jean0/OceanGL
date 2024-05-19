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
    float A;

    if (FBM == 0) {
        for (int i = 0; i < nbVagues; i++) {
            L = mix(L_min, L_max, rand(vec2(float(i), seed))); 
            w = (2.0 * PI) / L;
            A = mix(Amplitude_min, Amplitude_max, rand(vec2(float(i), seed)));
            float randomPhase = rand(vec2(float(i) + 0.1, seed)) * 2.0 * PI;
            vec3 randomDirection = normalize(reflect(Direction, normalize(vec3(
                rand(vec2(float(i) + 0.2, seed)), 
                rand(vec2(float(i) + 0.3, seed)), 
                rand(vec2(float(i) + 0.4, seed))))));

            float phi_t = (S * w) * time + randomPhase;
            float sinTerm = sin(dot(randomDirection, Pos) * w + phi_t);

            wave += A * sinTerm;
        }
    } else if (FBM == 1) {
        float wFBM = (2.0 * PI) / L_FBM;
        float AFBM = Amplitude_FBM;

        for (int i = 0; i < nbVagues; i++) {
            vec3 randomDirection = randomDir(float(i) + seed);
            float phi_t = (S * wFBM) * time;
            float sinTerm = sin(dot(randomDirection, Pos) * wFBM + phi_t);

            wave += AFBM * sinTerm;

            wFBM *= Gain_W;
            AFBM *= Gain_A;
        }
    }

    newPos.y += wave;   
    return newPos;
}

vec3 CalculateNormal(vec3 Pos, float S, vec3 Direction, float time, out vec3 dPosdx, out vec3 dPosdz) {
    float eps = 0.01; // Small offset for finite difference
    vec3 dX = vec3(eps, 0.0, 0.0);
    vec3 dZ = vec3(0.0, 0.0, eps);
    
    vec3 posX = Add_Wave(Pos + dX, S, Direction, time);
    vec3 posZ = Add_Wave(Pos + dZ, S, Direction, time);
    vec3 posY = Add_Wave(Pos, S, Direction, time);
    
    dPosdx = normalize(posX - posY);
    dPosdz = normalize(posZ - posY);
    
    return normalize(cross(dPosdz, dPosdx));
}

void main() {
    vec3 dPosdx, dPosdz;
    vec3 accPos = Add_Wave(aPos, S, Direction, time);
    vec3 accNor = CalculateNormal(aPos, S, Direction, time, dPosdx, dPosdz);

    vec3 newWave = accPos;
    gl_Position = projection * view * model * vec4(newWave, 1.0f);

    pos = newWave;
    normal = accNor;
    tangente = dPosdx;
    binormale = dPosdz;
    height = newWave.y;
    tex = aTex;
    normalWorld = normalize(mat3(transpose(inverse(model))) * normal); 
    positionWorld = (model * vec4(newWave, 1.0f)).xyz;
}