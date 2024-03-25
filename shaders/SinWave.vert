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


// Variables générales
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

uniform float Amplitude;
uniform float L;
uniform float S;
uniform vec3 Direction;
uniform float PI;


// Fonction pour une vague sinusoïdale
vec3 Add_Wave(vec3 Pos, float L, float S, float Amplitude, vec3 Direction, float time) {
    vec3 newPos = Pos; 
    float w = (2.0f * PI) / L;
    float phi_t = (S * w) * time;
    newPos.y = Amplitude * sin(dot(Direction.xyz, aPos.xyz) * w + phi_t);
    return newPos;
}

vec3 ComputeBinormale(vec3 Pos, float L, float S, float Amplitude, vec3 Direction, float time) {
    vec3 newPos = Pos; 
    float w = (2.0f * PI) / L;
    float phi_t = (S * w) * time;
    newPos.y = Amplitude * w * Direction.x * cos(dot(Direction.xyz, aPos.xyz) * w + phi_t);

    return normalize(vec3(1.0f, newPos.y, 0.f));
}

vec3 ComputeTangente(vec3 Pos, float L, float S, float Amplitude, vec3 Direction, float time) {
    vec3 newPos = Pos; 
    float w = (2.0f * PI) / L;
    float phi_t = (S * w) * time;
    newPos.y = Amplitude * w * Direction.z * cos(dot(Direction.xyz, aPos.xyz) * w + phi_t);

    return normalize(vec3(0.0f, newPos.y, 1.0f));
}

vec3 ComputeNormal(vec3 Pos, float L, float S, float Amplitude, vec3 Direction, float time) {
    vec3 newPos = Pos; 
    float w = (2.0f * PI) / L;
    float phi_t = (S * w) * time;
    newPos.x = (-1.0f) * Amplitude * w * Direction.x * cos(dot(Direction.xyz, aPos.xyz) * w + phi_t);
    newPos.z = (-1.0f) * Amplitude * w * Direction.z * cos(dot(Direction.xyz, aPos.xyz) * w + phi_t);

    return normalize(vec3(newPos.x, 1.0f, newPos.z));
}

void main()
{   
    vec3 newWave = Add_Wave(aPos, L, S, Amplitude, Direction, time);
    gl_Position = projection * view * model * vec4(newWave, 1.0f);

    // Binormale / Tangente / Normale
    binormale = ComputeBinormale(aPos, L, S, Amplitude, Direction, time);
    tangente = ComputeTangente(aPos, L, S, Amplitude, Direction, time);
    normal = ComputeNormal(aPos, L, S, Amplitude, Direction, time);
    // Pareil que :
    //normal = cross(tangente, binormale);


    // Matériau
    height = newWave.y;

    // Autres
    pos = newWave;
    tex = aTex;
}