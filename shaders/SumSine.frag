#version 460 core

in float height;

out vec4 FragColor;

void main() {
    vec3 colBase = vec3(0.6, 0.8, 0.98); // Crête
    vec3 ColClaire = vec3(0.01, 0.36, 0.7); // Creux

    float gradient = 0.2;

    float mixF = smoothstep(gradient, gradient + 0.1, height);
    vec3 fin = mix(colBase, ColClaire, mixF);
    FragColor = vec4(fin,1.0);
}