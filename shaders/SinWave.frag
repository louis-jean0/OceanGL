#version 330 core
  
out vec4 FragColor;

in float height;
in vec3 pos;
in vec2 tex;
in vec3 normal;
in vec3 binormale;
in vec3 tangente;

uniform int Debug;
uniform vec3 lightPos;
uniform vec3 viewPos;
uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;

void main() {

    // Ambient
    vec3 ambient = ambientStrength * vec3(0.0,0.0,1.0);

    // Diffuse
    vec3 normalizedNormal = normalize(normal);
    vec3 lightDir = normalize(lightPos - pos);
    float diff = max(dot(normalizedNormal, lightDir), 0.0);
    vec3 diffuse = diff * vec3(diffuseStrength);

    // Specular    
    vec3 viewDir = normalize(viewPos - pos);
    vec3 reflectDir = reflect(lightDir, normalizedNormal);  
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 128);
    vec3 specular = specularStrength * spec * vec3(1.0);
            
    vec3 result = (ambient + diffuse + specular);

    if(Debug == 0) {
        FragColor = vec4(result, 1.0);
    } else if(Debug == 1) {
        FragColor = vec4(pos, 1.0);
    } else if(Debug == 2) {
        FragColor = vec4(tex, 0.0, 1.0);
    } else if(Debug == 3) {
        FragColor = vec4(normal, 1.0);
    } else if(Debug == 4) {
        FragColor = vec4(binormale, 1.0);
    } else if(Debug == 5) {
        FragColor = vec4(tangente, 1.0);
    }
}