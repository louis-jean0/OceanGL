#version 430 core

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

// ----------------------------------------------------------------------------
//
// uniforms
//
// ----------------------------------------------------------------------------

layout(rgba32f, binding = 0) uniform image2D imgOutput;

uniform float time;                

// ----------------------------------------------------------------------------
//
// functions
//
// ----------------------------------------------------------------------------

void main() {
    vec4 value = vec4(0.0, 0.0, 0.0, 1.0);
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
    float speed = 100.0; // Ajout du point décimal pour la clarté
    float width = 512.0; // Ajout du point décimal pour la clarté

    // Utilisez la variable time pour modifier la coordonnée y
    value.x = mod(float(texelCoord.x) + time * speed, width) / (gl_NumWorkGroups.x * gl_WorkGroupSize.x);
    value.y = (float(texelCoord.y) ) / (gl_NumWorkGroups.y * gl_WorkGroupSize.y);
    imageStore(imgOutput, texelCoord, value);
}
