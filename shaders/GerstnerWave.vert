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
uniform float Amplitude;
uniform float g;
uniform float Wavelength;
uniform float Steepness;
uniform float Speed;
uniform vec3 Direction;

int nbVagues = 1;

// Fonction pour une vague basée sur le modèle de Gerstner
vec3 Add_Wave(vec3 Pos, float g, float PI, float L, float Steepness, float Amplitude, float S, vec3 Direction, float time) {
    vec3 newPos = aPos;

    float w = sqrt(g * ((2. * PI) / L));  
    float Q = Steepness / (w * Amplitude);
    float phi_t = (S * (2. / L)) * time;

    newPos.x += ((Q * Amplitude) * Direction.x * cos((w * dot(Direction.xyz, aPos.xyz)) + phi_t));
    newPos.z += ((Q * Amplitude) * Direction.z * cos((w * dot(Direction.xyz, aPos.xyz)) + phi_t));
    newPos.y = Amplitude * sin(w * dot(Direction.xyz, aPos.xyz) + phi_t);

    return newPos;
}


vec3 compute_Binormal(vec3 newPos, float g, float PI, float L, float Steepness, float Amplitude, float S, vec3 Direction, float time) {
    vec3 binormal = newPos;

    float w = sqrt(g * ((2. * PI) / L));  
    float Q = Steepness / (w * Amplitude);
    float phi_t = (S * (2. / L)) * time;

    float WA = w * Amplitude;
    float SA = sin(w * dot(Direction.xyz, aPos.xyz) + phi_t);
    float CA = cos(w * dot(Direction.xyz, aPos.xyz) + phi_t);

    binormal.x = 1. - (Q * (Direction.x * Direction.x) * WA * SA);
    binormal.z = 0. - (Q * (Direction.x * Direction.z) * WA * SA);
    binormal.y = Direction.x * WA * CA;

    return normalize(binormal);
}

vec3 compute_Tangent(vec3 newPos, float g, float PI, float L, float Steepness, float Amplitude, float S, vec3 Direction, float time) {
    vec3 tangent = newPos;

    float w = sqrt(g * ((2. * PI) / L));  
    float Q = Steepness / (w * Amplitude);
    float phi_t = (S * (2. / L)) * time;

    float WA = w * Amplitude;
    float SA = sin(w * dot(Direction.xyz, aPos.xyz) + phi_t);
    float CA = cos(w * dot(Direction.xyz, aPos.xyz) + phi_t);

    tangent.x = 0. - (Q * (Direction.x * Direction.z) * WA * SA);
    tangent.z = 1. - (Q * (Direction.z * Direction.z) * WA * SA);
    tangent.y = Direction.z * WA * CA;

    return normalize(tangent);
}

vec3 compute_Normal(vec3 newPos, float g, float PI, float L, float Steepness, float Amplitude, float S, vec3 Direction, float time) {
    vec3 normal = newPos;

    float w = sqrt(g * ((2. * PI) / L));  
    float Q = Steepness / (w * Amplitude);
    float phi_t = (S * (2. / L)) * time;

    float WA = w * Amplitude;
    float SA = sin(w * dot(Direction.xyz, aPos.xyz) + phi_t);
    float CA = cos(w * dot(Direction.xyz, aPos.xyz) + phi_t);

    normal.x = 0. - (Direction.x * WA * CA);
    normal.z = 0. - (Direction.z * WA * CA);
    normal.y = 1. - (Q * WA * SA);

    return normalize(normal);
}


void main()
{
    vec3 newWave = Add_Wave(aPos, g, PI, Wavelength, Steepness, Amplitude, Speed, Direction, time);
    gl_Position = projection * view * model * vec4(newWave, 1.0f);


    // Binormale / Tangente / Normale
    binormale = compute_Binormal(aPos, g, PI, Wavelength, Steepness, Amplitude, Speed, Direction, time);
    tangente = compute_Tangent(aPos, g, PI, Wavelength, Steepness, Amplitude, Speed, Direction, time);
    normal = compute_Normal(aPos, g, PI, Wavelength, Steepness, Amplitude, Speed, Direction, time);
    // Pareil que :
    //normal = cross(tangente, binormale);

    // Matériau
    height = newWave.y;

    // Autres
    pos = newWave;
    tex = aTex;
}