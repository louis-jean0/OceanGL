#version 330 core

out vec4 FragColor;
in vec3 TexCoords;

uniform samplerCube skyboxTexture;

void main()
{    
        //vec3 text =  texture(skyboxTexture, TexCoords).rgb;
        FragColor = texture(skyboxTexture, TexCoords);
}