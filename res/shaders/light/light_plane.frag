#version 460 core

uniform sampler2D texture0;

uniform vec3 light_position;
uniform vec3 light_color;

uniform float ambient_factor;

in vec2 uv;
in vec3 normal;
in vec3 fragment_position;

layout(location = 0) out vec4 FragColor;

void main()
{
//  ambient
    vec3 ambient = ambient_factor * light_color;

//  diffuse
    vec3 norm = normalize(normal);
    vec3 light_direction = normalize(fragment_position - light_position);
    float diff = max(dot(norm, light_direction), 0.f);
    vec3 diffuse = light_color * diff;

    FragColor = ( vec4(ambient, 1.f) + vec4(diffuse, 1.f) ) * texture(texture0, uv);
}