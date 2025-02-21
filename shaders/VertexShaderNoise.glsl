#version 330

// Input attributes from the vertex buffer
layout(location = 0) in vec3 v_position;
layout(location = 1) in vec3 v_normal;
layout(location = 2) in vec2 v_texture_coord;

// Uniforms for transformation matrices
uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform vec3 light_position;
uniform vec3 eye_position;
uniform float material_kd;
uniform float material_ks;
uniform int material_shininess;
uniform vec3 object_color;

uniform float frequency;
uniform float amplitude;

out vec3 color;

float noise(vec2 position) {
    return mix(fract(sin(dot(position, vec2(12.9898, 78.233))) * 458.5453), 0.1, 0.05);
}

void main()
{
    vec2 noise_position = v_position.xz * frequency;
    float noise_value = noise(noise_position);

    float height_offset = noise_value * amplitude;

    vec3 modified_position = v_position;
    modified_position.y += height_offset;

    vec3 low_color = vec3(0.427, 0.659, 0.26);
    vec3 high_color = vec3(0.522, 0.341, 0.04);

    vec3 world_pos = (Model * vec4(modified_position, 1.0)).xyz;
    vec3 world_normal = normalize(mat3(Model) * v_normal);
    float ambient_light = 0.15;

    vec3 L = normalize(light_position - world_pos);
    vec3 V = normalize(eye_position - world_pos);
    float diffuse_light = max(dot(world_normal, L), 0.0);

    vec3 H = normalize(L + V);
    float specular_light = 0;

    specular_light = pow(max(dot(world_normal, H), 0.0), material_shininess);
    float dist = length(light_position - world_pos.xyz);
    float aten = 30.0 / (0.2 + 0.03 * dist + 0.012 * dist * dist);

    vec3 light_intensity = ambient_light * object_color + 
                           diffuse_light * material_kd * object_color +
                           specular_light * material_ks * vec3(1.0);

    color = mix(low_color, light_intensity * aten, noise_value);
    gl_Position = Projection * View * Model * vec4(modified_position, 1.0);
}
