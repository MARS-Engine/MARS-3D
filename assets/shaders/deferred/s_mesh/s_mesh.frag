#version 440 core
layout (location = 0) out vec4 position;
layout (location = 1) out vec4 normal;
layout (location = 2) out vec4 albedo;

layout (location = 0) in vec3 Position;
layout (location = 1) in vec2 TexUV;
layout (location = 2) in vec3 Normal;

layout(binding = 1) uniform sampler2D tex_diffuse;
layout(binding = 2) uniform sampler2D tex_specular;

void main() {
    position = vec4(Position, 1.0);
    normal = vec4(normalize(Normal), 1.0);
    albedo.rgb = texture(tex_diffuse, TexUV).rgb;
    albedo.a = texture(tex_specular, TexUV).r;
}