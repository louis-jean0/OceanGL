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
uniform samplerCube skyboxTexture;

void main() {
    vec3 colBase = vec3(0.6, 0.8, 0.98); // Crête
    vec3 colCreux = vec3(0.01, 0.36, 0.7); // Creux

    float gradient = 0.8;
    float mixF = smoothstep(gradient, gradient + 0.1, height);
    vec3 baseColor = mix(colBase, colCreux, mixF);

    // Phong lighting model

    // Ambient
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 lightDir = normalize(lightPosition - pos);
    float diff = max(dot(normal, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;

    // Specular
    vec3 viewDir = normalize(viewPosition - pos);
    vec3 reflectDir = reflect(-viewDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), 32);
    vec3 specular = specularStrength * spec * lightColor * 0.3;

    // Fresnel effect
    const float FresnelPower = 1.0;
    const float FresnelBias = 0.1;
    const float FresnelScale = 0.1;
    vec3 fresnelNormal = normalize(vec3(normal.x * FresnelScale, normal.y, normal.z * FresnelScale));
    float fresnelFactor = pow(1.0 - dot(viewDir, fresnelNormal), FresnelPower) + FresnelBias;
    vec3 fresnelColor = fresnelFactor * lightColor;

    // Final color computation
    vec3 skyboxColor = texture(skyboxTexture, reflectDir).rgb;
    vec3 finalColor = ambient + diffuse + specular + fresnelColor;
    finalColor = mix(baseColor, finalColor, 0.7);
    finalColor = mix(finalColor, skyboxColor, fresnelFactor);
    finalColor = clamp(finalColor, 0.0, 1.0);

    if(Debug == 0) {
        FragColor = vec4(finalColor, 1.0);
    } else if(Debug == 1) {
        FragColor = vec4(pos * 0.5 + 0.5, 1.0);
    } else if(Debug == 2) {
        FragColor = vec4(tex, 0.0, 1.0);
    } else if(Debug == 3) {
        FragColor = vec4(normal * 0.5 + 0.5, 1.0);
    } else if(Debug == 4) {
        FragColor = vec4(binormale * 0.5 + 0.5, 1.0);
    } else if(Debug == 5) {
        FragColor = vec4(tangente * 0.5 + 0.5, 1.0);
    }
}