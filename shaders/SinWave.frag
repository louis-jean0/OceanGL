#version 460 core
  
out vec4 FragColor;

in float height;
in vec3 pos;
in vec2 tex;
in vec3 normal;
in vec3 binormale;
in vec3 tangente;

// PBR In
in vec3 TangentLightPos;
in vec3 TangentViewPos;
in vec3 TangentFragPos;
in vec4 positionWorld;
in vec3 o_positionWorld;
in vec3 o_normalWorld;

const float PI = 3.14159265359;

uniform samplerCube skyboxTexture;

// Functions
// ----------------------------------------------------------------------------
float DistributionGGX(vec3 N, vec3 H, float roughness)
{
    float a = roughness*roughness;
    float a2 = a*a;
    float NdotH = max(dot(N, H), 0.0);
    float NdotH2 = NdotH*NdotH;

    float nom   = a2;
    float denom = (NdotH2 * (a2 - 1.0) + 1.0);
    denom = PI * denom * denom;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySchlickGGX(float NdotV, float roughness)
{
    float r = (roughness + 1.0);
    float k = (r*r) / 8.0;

    float nom   = NdotV;
    float denom = NdotV * (1.0 - k) + k;

    return nom / denom;
}
// ----------------------------------------------------------------------------
float GeometrySmith(vec3 N, vec3 V, vec3 L, float roughness)
{
    float NdotV = max(dot(N, V), 0.0);
    float NdotL = max(dot(N, L), 0.0);
    float ggx2 = GeometrySchlickGGX(NdotV, roughness);
    float ggx1 = GeometrySchlickGGX(NdotL, roughness);

    return ggx1 * ggx2;
}
// ----------------------------------------------------------------------------
vec3 fresnelSchlick(float cosTheta, vec3 F0)
{
    return F0 + (1.0 - F0) * pow(clamp(1.0 - cosTheta, 0.0, 1.0), 5.0);
}
// ----------------------------------------------------------------------------

uniform int Debug;

// Light
uniform vec3 ViewPos;
uniform float roughness;
uniform float ao;
uniform float metallic;
uniform vec3 albedo;

void main() {
    vec3 N = normalize(o_normalWorld);
    vec3 V = normalize(TangentViewPos - TangentFragPos);
    vec3 L = normalize(TangentLightPos - TangentFragPos);
    vec3 H = normalize(V + L);

    vec3 F0 = vec3(0.04); 
    F0 = mix(F0, albedo, metallic);

    vec3 envColor = texture(skyboxTexture, reflect(-V, N)).rgb;

    float NDF = DistributionGGX(N, H, roughness);
    float G = GeometrySmith(N, V, L, roughness);
    vec3 F = fresnelSchlick(max(dot(H, V), 0.0), F0);

    vec3 numerator = NDF * G * F;
    float denominator = 4.0 * max(dot(N, V), 0.0) * max(dot(N, L), 0.0) + 0.0001;
    vec3 specular = numerator / denominator;

    vec3 kS = F;
    vec3 kD = (1.0 - kS) * (1.0 - metallic);

    float NdotL = max(dot(N, L), 0.0);

    vec3 Lo = (kD * albedo / PI + specular) * envColor * NdotL;

    vec3 ambient = vec3(0.03) * albedo * ao;
    vec3 color = ambient + Lo;


    if(Debug == 0) {
        FragColor = vec4(color, 1.0);
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
