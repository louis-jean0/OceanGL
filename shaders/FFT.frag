#version 460 core
  
out vec4 FragColor;

in vec2 texCoords;

uniform sampler2D tex;

void main() {
    vec3 text = texture(tex, texCoords).rgb;
    FragColor = vec4(text, 1.);
}
