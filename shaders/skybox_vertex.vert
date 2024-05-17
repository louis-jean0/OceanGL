#version 460 core

layout (location = 0) in vec3 aPos;

uniform mat4 model;
uniform mat4 projection;
uniform mat4 view;

uniform int res;

out vec3 TexCoords;

void main()
{       
        TexCoords = aPos;
        vec3 scale = aPos * res;
        gl_Position = projection * view * model * vec4(scale, 1.0);
} 
