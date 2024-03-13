#version 330 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;

// Colors vertices
out float height;


// General variables
uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;


uniform float Amplitude;
uniform float L;
uniform float S;
uniform vec3 Direction;

void main()
{
    vec3 newPos = aPos; 
    float w = 2.0f / L;
    float p = S * w;
    newPos.y = Amplitude * sin(dot(Direction.xyz, aPos.xyz) * w + time * p);
    height = newPos.y;
    gl_Position = projection * view * model * vec4(newPos, 1.0f);
}