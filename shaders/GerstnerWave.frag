// #version 330 core
  
// out vec4 FragColor;

// in float height;
// in vec3 pos;
// in vec2 tex;
// in vec3 normal;
// in vec3 binormale;
// in vec3 tangente;

// uniform int Debug;

// void main() {
//     vec3 colBase = vec3(0.6, 0.8, 0.98); // Crête
//     vec3 ColClaire = vec3(0.01, 0.36, 0.7); // Creux

//     float gradient = 0.2;

//     float mixF = smoothstep(gradient, gradient + 0.1, height);
//     vec3 fin = mix(colBase, ColClaire, mixF);

//     if(Debug == 0) {
//         FragColor = vec4(fin, 1.0);
//     } else if(Debug == 1) {
//         FragColor = vec4(pos, 1.0);
//     } else if(Debug == 2) {
//         FragColor = vec4(tex, 0.0, 1.0);
//     } else if(Debug == 3) {
//         FragColor = vec4(normal, 1.0);
//     } else if(Debug == 4) {
//         FragColor = vec4(binormale, 1.0);
//     } else if(Debug == 5) {
//         FragColor = vec4(tangente, 1.0);
//     }
// }
#version 330 core

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
uniform samplerCube skyboxTexture;

void main() {
    vec3 colBase = vec3(0.6, 0.8, 0.98); // Crête
    vec3 colCreux = vec3(0.01, 0.36, 0.7); // Creux

    float gradient = 0.2;
    float mixF = smoothstep(gradient, gradient + 0.1, height);
    vec3 baseColor = mix(colBase, colCreux, mixF);

    // Ensure normals are normalized
    vec3 N = normalize(normal);
    vec3 T = normalize(tangente);
    vec3 B = normalize(binormale);

    // Phong lighting model

    // Ambient
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 lightDir = normalize(lightPosition - pos);
    float diff = max(dot(N, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;

    // Specular
    vec3 viewDir = normalize(viewPosition - pos);
    vec3 reflectDir = reflect(-viewDir, N);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(N, halfwayDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor;

    // Fresnel effect
    const float FresnelPower = 5.0;
    const float FresnelBias = 0.1;
    const float FresnelScale = 1.0;
    vec3 fresnelNormal = normalize(vec3(N.x * FresnelScale, N.y, N.z * FresnelScale));
    float fresnelFactor = pow(1.0 - dot(viewDir, fresnelNormal), FresnelPower) + FresnelBias;
    vec3 fresnelColor = fresnelFactor * lightColor;

    // Sample skybox texture
    vec3 skyboxColor = texture(skyboxTexture, reflectDir).rgb;

    // Combine lighting and skybox reflection
    vec3 finalColor = ambient + diffuse + specular + fresnelColor;
    finalColor = mix(baseColor, finalColor, 0.7);
    finalColor = mix(finalColor, skyboxColor, fresnelFactor * 0.5);

    // Clamp final color to avoid negative values
    finalColor = clamp(finalColor, 0.0, 1.0);

    if(Debug == 0) {
        FragColor = vec4(finalColor, 1.0);
    } else if(Debug == 1) {
        FragColor = vec4(pos * 0.5 + 0.5, 1.0);
    } else if(Debug == 2) {
        FragColor = vec4(tex, 0.0, 1.0);
    } else if(Debug == 3) {
        FragColor = vec4(N * 0.5 + 0.5, 1.0); // Visualize normal
    } else if(Debug == 4) {
        FragColor = vec4(B * 0.5 + 0.5, 1.0); // Visualize binormal
    } else if(Debug == 5) {
        FragColor = vec4(T * 0.5 + 0.5, 1.0); // Visualize tangent
    }
}