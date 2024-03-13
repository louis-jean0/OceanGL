#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

// Colors vertices
out float Height;
out vec3 pos;
out vec3 normal;

// General variables
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

uniform float PI;
uniform float Amplitude;
uniform float g;
uniform float L;
uniform float Steepness;
uniform vec3 Direction;

out float height;

void main()
{
    vec3 wave_position = vec3(aPos.x, 0, aPos.z);

    float w = sqrt(g * ((2. * PI) / L));  
    float Q = Steepness / (w * Amplitude);

    vec3 newPos = aPos; 
    newPos.x += ((Q * Amplitude) * Direction.x * cos((w * dot(Direction.xyz, aPos.xyz)) + time));
    newPos.z += ((Q * Amplitude) * Direction.z * cos((w * dot(Direction.xyz, aPos.xyz)) + time));
    newPos.y = Amplitude * sin(w * dot(Direction.xyz, aPos.xyz) + time);

    height = newPos.y;
    gl_Position = projection * view * model * vec4(newPos, 1.0);
}
