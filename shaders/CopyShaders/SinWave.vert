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


uniform float amplitude;
uniform float x;
uniform float k;
uniform float af;
uniform float p;



void main()
{
    vec3 newPos = aPos; 
    newPos.y = amplitude * sin(k * x * aPos.x + af * time + p);
    height = newPos.y;
    gl_Position = projection * view * model * vec4(newPos, 1.0f);
}