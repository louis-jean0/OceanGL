#version 460 core
  
out vec4 FragColor;

in float height;
in vec3 pos;
in vec2 tex;
in vec3 normal;
in vec3 binormale;
in vec3 tangente;
in vec3 positionWorld;
in vec3 normalWorld;

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
    vec3 ColClaire = vec3(0.01, 0.36, 0.7); // Creux

    float gradient = 0.8;

    float mixF = smoothstep(gradient, gradient + 0.1, height);
    //vec3 fin = mix(colBase, ColClaire, mixF);

    // Phong

    // Ambient
    vec3 ambient = ambientStrength * lightColor;

    // Diffuse
    vec3 lightDir = normalize(lightPosition - pos);
    float diff = max(dot(normalWorld, lightDir), 0.0);
    vec3 diffuse = diffuseStrength * diff * lightColor;

    // Specular
    vec3 viewDir = normalize(viewPosition - pos);
    vec3 reflectDir = normalize(reflect(-lightDir, normalWorld));
    // float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    // vec3 specular = specularStrength * spec * lightColor;

    //vec3 fin = (ambient + diffuse + specular) + mixF;

    // vec3 incident = normalize(positionWorld - viewPos);
    // vec3 adjustedNormal = normalize(mix(normalWorld, vec3(0, 1, 0), positionWorld.y * 0.3));
    // vec3 reflected = reflect(incident, normalWorld);
    // vec3 skyColor = texture(skyboxTexture,reflected).rgb;
    // vec3 waterColor = vec3(0.0,0.3,0.4);
    // float reflectionStrength = clamp(1.0 - positionWorld.y * 0.9, 0.0, 1.0);
    // float fresnel = pow(1.0 - dot(normalize(incident), normalize(adjustedNormal)), 0.5);
    // vec3 finalColor = mix(waterColor, skyColor, fresnel * reflectionStrength);

    // Fresnel
    const float FresnelNormalStrength = 2.0;
    const float FresnelShininess = 1.0;  // Contrôle l'exponentielle du Fresnel
    const float FresnelBias = 0.1;       // Biais minimal pour l'effet Fresnel
    const float FresnelStrength = 1.0;   // Amplifie l'effet Fresnel total
    const vec3 FresnelColor = vec3(0.6, 0.6, 0.6);  // Couleur de base de l'effet Fresnel
    vec3 fresnelNormal = normalize(vec3(normalWorld.x * FresnelNormalStrength, normalWorld.y, normalWorld.z * FresnelNormalStrength));
    float base = 1.0 - dot(viewDir, fresnelNormal);
    float exponential = pow(base, FresnelShininess);
    float R = clamp(exponential + FresnelBias * (1.0 - exponential), 0.0, 1.0);
    vec3 fresnel = FresnelColor * R;
    vec3 skyCol = texture(skyboxTexture, reflectDir).rgb;
    vec3 sunColor = vec3(0.8,0.8,0.3);
    vec3 sun = sunColor * pow(max(0.0, dot(reflectDir, lightDir)), 500.0);
    fresnel = skyCol * R;
    fresnel += sun * R;
    //fresnel = mix(skyCol, vec3(0.0, 0.3, 0.4), fresnel);
    vec3 tipColor = vec3(0.5,0.5,0.5) * pow(height, 10.0);
    float specularStrength = 0.8;
    float specularExponent = 200.0;
    float specularNormalStrength = 0.5;
    vec3 specNormal = normalize(vec3(normalWorld.x * specularNormalStrength, normal.y, normal.z * specularNormalStrength));
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(dot(specNormal, halfwayDir), specularExponent);
    vec3 specular = lightColor * specularStrength * spec;

    base = 1.0 - dot(viewDir, halfwayDir);
    exponential = pow(base, 5.0);
    R = exponential + FresnelBias * (1.0 - exponential);
    specular *= R;
    vec3 finalColor = ambient + diffuse + specular + fresnel + tipColor;

    if(Debug == 0) {
        FragColor = vec4(finalColor, 1.0);
    } else if(Debug == 1) {
        FragColor = vec4(pos, 1.0);
    } else if(Debug == 2) {
        FragColor = vec4(tex, 0.0, 1.0);
    } else if(Debug == 3) {
        FragColor = vec4(normalWorld, 1.0);
    } else if(Debug == 4) {
        FragColor = vec4(binormale, 1.0);
    } else if(Debug == 5) {
        FragColor = vec4(tangente, 1.0);
    }
}
