#pragma once

#include "../shadow/pcss.glsl"
#include "../shadow/vssm.glsl"

float evaluate_directional_shadow(int index, vec3 world_pos, float bias)
{
    mat4 light_matrix = directional_light_matrix[index];

    vec4 pos_light_space = light_matrix * vec4(world_pos, 1.0);
    vec3 shadow_coord = (pos_light_space.xyz / pos_light_space.w + 1.0) / 2.0;

    float visibility = 1.0;
    if (shadow_method == SHADOW_BASIC) {
        visibility = shadow_mapping(directional_shadow_map[index], shadow_coord);
    } else if (shadow_method == SHADOW_PCF) {
        visibility = PCF(directional_shadow_map[index], shadow_coord, PCF_PENUMBRA);
    } else if (shadow_method == SHADOW_PCSS) {
        visibility = PCSS(directional_shadow_map[index], shadow_coord);
    } else if (shadow_method == SHADOW_VSM) {
        visibility = VSM(directional_shadow_map[index], directional_SAT_map[index], shadow_coord, VSM_PENUMBRA);
    } else if (shadow_method == SHADOW_VSSM) {
        visibility = VSSM(directional_shadow_map[index], directional_SAT_map[index], shadow_coord, bias);
    }

    return visibility;
}

float evaluate_spot_shadow(int index, vec3 world_pos, float bias)
{
    mat4 light_matrix = spot_light_matrix[index];

    vec4 pos_light_space = light_matrix * vec4(world_pos, 1.0);
    vec3 shadow_coord = (pos_light_space.xyz / pos_light_space.w + 1.0) / 2.0;

    float visibility = 1.0;
    if (shadow_method == SHADOW_BASIC) {
        visibility = shadow_mapping(spot_shadow_map[index], shadow_coord);
    } else if (shadow_method == SHADOW_PCF) {
        visibility = PCF(spot_shadow_map[index], shadow_coord, PCF_PENUMBRA);
    } else if (shadow_method == SHADOW_PCSS) {
        visibility = PCSS(spot_shadow_map[index], shadow_coord);
    } else if (shadow_method == SHADOW_VSM) {
        visibility = VSM(spot_shadow_map[index], spot_SAT_map[index], shadow_coord, VSM_PENUMBRA);
    } else if (shadow_method == SHADOW_VSSM) {
        visibility = VSSM(spot_shadow_map[index], spot_SAT_map[index], shadow_coord, bias);
    }

    return visibility;
}
