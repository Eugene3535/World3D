#version 460 core

uniform sampler2D texture0;

uniform vec3 light_position;
uniform vec3 light_color;

uniform float ambient_factor;
uniform float specular_factor;
uniform float shininess;

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
    vec3 light_dir = normalize(light_position - fragment_position);
    vec3 diffuse = light_color * max(dot(norm, light_dir), 0.f);

//  specular
    vec3 view_dir = normalize(-fragment_position);
    vec3 reflect_dir = reflect(-light_dir, norm);
    float specular_value = pow(max(dot(view_dir, reflect_dir), 0.0), shininess);
    vec3 specular = specular_factor * specular_value * light_color;

    FragColor = vec4(ambient + diffuse + specular, 1.f) * texture(texture0, uv);
}