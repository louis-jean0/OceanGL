#version 460 core
  
out vec4 FragColor;

in float height;
in vec3 pos;
in vec2 tex;
in vec3 normal;
in vec3 binormale;
in vec3 tangente;

uniform int Debug;
uniform vec3 lightPosition;
uniform vec3 lightColor;
uniform vec3 viewPosition;
uniform float ambientStrength;
uniform float diffuseStrength;
uniform float specularStrength;

void main() {
    vec3 colBase = vec3(0.6, 0.8, 0.98); // Crête
    vec3 ColClaire = vec3(0.01, 0.36, 0.7); // Creux

    float gradient = 0.2;

    float mixF = smoothstep(gradient, gradient + 0.1, height);
    //vec3 fin = mix(colBase, ColClaire, mixF);

    // Phong

    // Ambient
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 lightDir = normalize(lightPosition - pos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;

    // Specular
    vec3 viewDir = normalize(viewPosition - pos);
    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    vec3 fin = (ambient + diffuse + specular);

    if(Debug == 0) {
        FragColor = vec4(fin, 1.0);
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
