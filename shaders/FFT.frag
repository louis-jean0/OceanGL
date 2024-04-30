#version 460 core
  
out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D butterflyTexture;

void main() {
    vec3 text = texture(butterflyTexture, texCoords).rgb;
    FragColor = vec4(text, 1.);
}
