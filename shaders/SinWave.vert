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

uniform float Amplitudes[5];
uniform float Lengths[5];
uniform float Speeds[5];
uniform vec3 Directions[5];
uniform int waveCount;

// Fonction pour du random
float random (vec2 st) {
    return fract(sin(dot(st.xy,
                         vec2(12.9898,78.233)))*
        43758.5453123);
}

// Fonction pour une vague sinusoïdale
vec3 Add_Wave(vec3 Pos, float L, float S, float Amplitude, vec3 Direction, float time) {
    vec3 newPos = Pos; 
    float w = 2.0f / L;
    float phi_t = (S * w) * time;
    newPos.y = Amplitude * sin(dot(Direction.xyz, aPos.xyz) * w + phi_t);
    return newPos;
}

// Dérivée partielle : df/dx
float Add_Wave_dx(vec3 Pos, float L, float S, float Amplitude, vec3 Direction, float delta, float time) {
    vec3 newPos_dx = Pos + vec3(delta, 0.0, 0.0); 
    return (Add_Wave(newPos_dx, L, S, Amplitude, Direction, time).y - Add_Wave(Pos, L, S, Amplitude, Direction, time).y) / delta;
}

// Dérivée partielle : df/dz
float Add_Wave_dz(vec3 Pos, float L, float S, float Amplitude, vec3 Direction, float delta, float time) {
    vec3 newPos_dz = Pos + vec3(0.0, 0.0, delta); 
    return (Add_Wave(newPos_dz, L, S, Amplitude, Direction, time).y - Add_Wave(Pos, L, S, Amplitude, Direction, time).y) / delta;
}

void main()
{   
    vec3 position = aPos;
    vec3 totalNormal = vec3(0.0, 1.0, 0.0);

    // Accumulation des effets de chaque vague
    for (int i = 0; i < waveCount; ++i) {
        position += Add_Wave(aPos, Lengths[i], Speeds[i], Amplitudes[i], Directions[i], time);
        totalNormal += vec3(Add_Wave_dx(aPos, Lengths[i], Speeds[i], Amplitudes[i], Directions[i], 0.0001, time), 0.0, Add_Wave_dz(aPos, Lengths[i], Speeds[i], Amplitudes[i], Directions[i], 0.0001, time));
    }

    // Normalisation de la normale totale pour s'assurer qu'elle reste unitaire
    totalNormal = normalize(totalNormal);

    // Calcul de gl_Position et des sorties
    gl_Position = projection * view * model * vec4(position, 1.0);
    normal = totalNormal;
    height = position.y;
    pos = position;
    tex = aTex;

    // Binormale / Tangente / Normale
    // float delta = 0.0000001;
    // binormale = vec3(1., Add_Wave_dx(aPos, L, S, Amplitude, Direction, delta, time), 0.0);
    // tangente = vec3(0., Add_Wave_dz(aPos, L, S, Amplitude, Direction, delta, time), 1.0);
    // normal = cross(tangente, binormale);

    normal = totalNormal;

}