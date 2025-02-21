#version 330

// Input
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniform properties
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform bool isShadow;

// Uniforms for light properties
uniform vec3 light_position;
uniform vec3 eye_position;
uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;

uniform vec3 object_color;

out vec3 color;


void main()
{
    vec3 world_pos = (Model * vec4(v_position, 1.0)).xyz;
    vec3 world_normal = normalize(mat3(Model) * v_normal);

    float ambient_light = 0.35;

    vec3 L = normalize(light_position - world_pos);
    vec3 V = normalize(eye_position - world_pos);
    float diffuse_light = max(dot(world_normal, L), 0.0);

    vec3 H = normalize(L + V);
    float specular_light = 0;

    specular_light = pow(max(dot(world_normal, H), 0.0), material_shininess);
    float dist = length(light_position - world_pos.xyz);
    float aten = 1.0 / (0.002 * dist + 0.0005 * dist * dist);
    vec3 light_intensity = ambient_light * object_color + 
                           diffuse_light * material_kd * object_color +
                           specular_light * material_ks * vec3(1.0);

    color = light_intensity * aten;
    gl_Position = Projection * View * Model * vec4(v_position, 1.0);
}
