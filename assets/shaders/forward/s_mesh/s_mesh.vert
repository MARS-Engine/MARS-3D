#version 440 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;

layout (location = 0) out vec2 TexCoord;

layout(binding = 0) uniform position {
    mat4 transform;
    mat4 model;
    mat3 normal;
} pos;


void main() {
    gl_Position = pos.transform * vec4(aPos, 1.0);
    TexCoord = aTexCoord;
}