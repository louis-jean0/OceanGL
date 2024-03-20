#version 330 core
  
out vec4 FragColor;

in float height;
in vec3 pos;
in vec2 tex;
in vec3 normal;
in vec3 binormale;
in vec3 tangente;

uniform int Debug;

uniform sampler2D nxTexture;
uniform sampler2D nyTexture;
uniform sampler2D nzTexture;
uniform sampler2D pxTexture;
uniform sampler2D pyTexture;
uniform sampler2D pzTexture;

void main() {
    vec3 colBase = vec3(0.6, 0.8, 0.98); // Crête
    vec3 ColClaire = vec3(0.01, 0.36, 0.7); // Creux

    float gradient = 0.2;

    float mixF = smoothstep(gradient, gradient + 0.1, height);
    vec3 fin = mix(colBase, ColClaire, mixF);

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
