#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTex;
layout (location = 2) in vec3 aNormal;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform float time;

uniform float amplitude;
uniform float frequence;

out float height;

void main() {

    float wave = sin(aPos.x * frequence);
    wave += sin(dot(vec3(0.2,0.0,1.0),aPos.xyz)*frequence*2.1 + time)*4.5;
    wave += sin(dot(vec3(0.9,0.0,0.3),aPos.xyz)*frequence*1.72 + time*1.121)*4.0;
    wave += sin(dot(vec3(0.7,0.0,0.5),aPos.xyz)*frequence*2.221 + time*0.437)*5.0;
    wave += sin(dot(vec3(0.1,0.0,0.3),aPos.xyz)*frequence*3.1122 + time*4.269)*2.5;
    wave *= amplitude*0.06;
    vec3 newPos = aPos + vec3(0, wave, 0);

    height = newPos.y;

    gl_Position = projection * view * model * vec4(newPos, 1.0);
}