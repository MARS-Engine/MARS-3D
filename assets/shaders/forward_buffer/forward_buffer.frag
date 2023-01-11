#version 440 core
layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 TexCoord;

layout(binding = 1) uniform sampler2D tex_diffuse;
layout(binding = 2) uniform sampler2D tex_specular;

void main() {
    FragColor = texture(tex_diffuse, TexCoord);
}