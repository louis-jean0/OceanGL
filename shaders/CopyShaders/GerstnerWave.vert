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
uniform float amplitude;
uniform float g;
uniform float L;
uniform float steepness;

out float height;

void main()
{
    vec3 wave_position = vec3(aPos.x, 0, aPos.z);

    float w = sqrt(g * ((2. * PI) / L));  
    float Q = steepness / (w * amplitude);
    vec3 D = normalize(vec3(1., 0., 0.));

    vec3 newPos = aPos; 
    newPos.x += ((Q * amplitude) * D.x * cos((w * dot(D.xyz, aPos.xyz)) + time));
    newPos.z += ((Q * amplitude) * D.z * cos((w * dot(D.xyz, aPos.xyz)) + time));
    newPos.y = amplitude * sin((w * dot(D.xyz, aPos.xyz)) + time);

    height = newPos.y;
    gl_Position = projection * view * model * vec4(newPos, 1.0);
}
