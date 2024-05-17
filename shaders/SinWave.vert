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

// PBR
out vec3 TangentLightPos;
out vec3 TangentViewPos;
out vec3 TangentFragPos;
out vec4 positionWorld;
out vec3 o_positionWorld;
out vec3 o_normalWorld;



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

// PBR
uniform vec3 LightPos;
uniform vec3 ViewPos;


// Fonction pour une vague sinusoïdale
vec3 Add_Wave(vec3 Pos, float L, float S, float Amplitude, vec3 Direction, float time) {
    vec3 newPos = Pos; 
    float w = (2.0f * PI) / L;
    float phi_t = (S * w) * time;
    newPos.y = Amplitude * sin(dot(Direction.xz, Pos.xz) * w + phi_t);
    return newPos;
}

vec3 ComputeBinormale(vec3 Pos, float L, float S, float Amplitude, vec3 Direction, float time) { 
    float w = (2.0f * PI) / L;
    float phi_t = (S * w) * time;
    float dx = Amplitude * w * Direction.x * cos(dot(Direction.xz, Pos.xz) * w + phi_t);

    return normalize(vec3(1.0f, dx, 0.f));
}

vec3 ComputeTangente(vec3 Pos, float L, float S, float Amplitude, vec3 Direction, float time) {
    float w = (2.0f * PI) / L;
    float phi_t = (S * w) * time;
    float dz = Amplitude * w * Direction.z * cos(dot(Direction.xz, Pos.xz) * w + phi_t);

    return normalize(vec3(0.0f, dz, 1.0f));
}

void main()
{   
    vec3 newWave = Add_Wave(aPos, L, S, Amplitude, Direction, time);
    //gl_Position = projection * view * model * vec4(newWave, 1.0f);

    // Binormale / Tangente / Normale
    binormale = ComputeBinormale(aPos, L, S, Amplitude, Direction, time);
    tangente = ComputeTangente(aPos, L, S, Amplitude, Direction, time);
    normal = normalize(cross(tangente,binormale));

    // Autres
    pos = newWave;
    tex = aTex;

    mat3 normalMatrix = mat3(transpose(inverse(model)));
    positionWorld = model * vec4(newWave, 1.0);
    o_positionWorld = positionWorld.xyz;
    o_normalWorld = normalMatrix * normal;

    vec3 Pos = vec3(model * vec4(newWave, 1.0));

    vec3 T = normalize(vec3(model * vec4(tangente, 0.0)));
    vec3 N = normalize(vec3(model * vec4(normal, 0.0)));
    T = normalize(T - dot(T, N) * N);
    vec3 B = cross(N, T);
        
    mat3 TBN = transpose(mat3(T, B, N));    
    TangentLightPos = TBN * LightPos;
    TangentViewPos  = TBN * ViewPos;
    TangentFragPos  = TBN * Pos;

    binormale = B;

    // Matériau
    height = newWave.y;

    gl_Position = projection * view * positionWorld;
}