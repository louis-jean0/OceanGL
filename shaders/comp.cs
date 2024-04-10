#version 430 core

layout (local_size_x = 16, local_size_y = 16, local_size_z = 1) in;

// ----------------------------------------------------------------------------
//
// uniforms
//
// ----------------------------------------------------------------------------

layout(rgba32f, binding = 0) uniform image2D tex;

uniform float time;                

// ----------------------------------------------------------------------------
//
// functions
//
// ----------------------------------------------------------------------------

void main() {
    vec4 value = vec4(0.0, 0.0, 0.0, 1.0);
    ivec2 texelCoord = ivec2(gl_GlobalInvocationID.xy);
    float speed = 5.0; // Ajout du point décimal pour la clarté
    float width = 512.0; // Ajout du point décimal pour la clarté

    // Utilisez la variable time pour modifier la coordonnée y
    float y = sin(0.1*texelCoord.x + time * speed); //sinusoide

    value.x = y;
    value.y = y;
    value.z = y;

    imageStore(tex, texelCoord, value);
}
