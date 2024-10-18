#pragma once

#include "../common/parameter.glsl"
#include "../util/math.glsl"

float evaluate_directional_shadow(int index, vec3 world_pos)
{
    mat4 light_matrix = directional_light_matrix[index];

    vec4 pos_light_space = light_matrix * vec4(world_pos, 1.0);
    vec3 shadow_coord = (pos_light_space.xyz / pos_light_space.w + 1.0) / 2.0;

    float shadow_depth = texture(directional_shadow_map[index], shadow_coord.xy).r;
    float current_depth = shadow_coord.z;

    return current_depth <= shadow_depth ? 1.0 : 0.0;
}
