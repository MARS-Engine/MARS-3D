#version 440 core
layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 TexCoord;

layout(binding = 0) uniform sampler2D ourTexture;

void main() {
    FragColor = texture(ourTexture, TexCoord);
}