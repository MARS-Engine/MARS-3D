#version 440 core
layout (location = 0) out vec4 FragColor;

layout (location = 0) in vec2 TexCoords;

layout (binding = 2) uniform sampler2D gPosition;
layout (binding = 3) uniform sampler2D gNormal;
layout (binding = 4) uniform sampler2D gAlbedoSpec;

struct Light {
    vec3 Position;
    vec3 Color;

    float Linear;
    float Quadratic;
    float Radius;
};

const int MAX_LIGHTS = 32;
layout (binding = 0) uniform lights_data {
    Light lights[MAX_LIGHTS];
    int active_lights;
} lights;

layout (binding = 1) uniform camera_data {
    vec3 viewPos;
} cmaera;

void main()
{
    // retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;

    // then calculate lighting as usual
    vec3 lighting  = Diffuse * 0.1; // hard-coded ambient component
    vec3 viewDir  = normalize(cmaera.viewPos - FragPos);
    for(int i = 0; i < MAX_LIGHTS; ++i)
    {
        if (i >= lights.active_lights)
                break;
        // calculate distance between light source and current fragment
        float distance = length(lights.lights[i].Position - FragPos);
        if(distance < lights.lights[i].Radius)
        {
            // diffuse
            vec3 lightDir = normalize(lights.lights[i].Position - FragPos);
            vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights.lights[i].Color;
            // specular
            vec3 halfwayDir = normalize(lightDir + viewDir);
            float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
            vec3 specular = lights.lights[i].Color * spec * Specular;
            // attenuation
            float attenuation = 1.0 / (1.0 + lights.lights[i].Linear * distance + lights.lights[i].Quadratic * distance * distance);
            diffuse *= attenuation;
            specular *= attenuation;
            lighting += diffuse + specular;
        }
    }
    FragColor = vec4(lighting, 1.0);
}

