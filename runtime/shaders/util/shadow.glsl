#pragma once

#include "../common/parameter.glsl"
#include "../util/math.glsl"

vec2 poisson_disk[NUM_SAMPLES];
void poisson_disk_samples(const in vec2 random_seed) {

    float ANGLE_STEP = DOU_PI * float(NUM_RINGS) / float(NUM_SAMPLES);
    float INV_NUM_SAMPLES = 1.0 / float(NUM_SAMPLES);

    float angle = rand_2to1(random_seed) * DOU_PI;
    float radius = INV_NUM_SAMPLES;
    float radius_step = radius;

    for( int i = 0; i < NUM_SAMPLES; i ++ ) {
        poisson_disk[i] = vec2(cos(angle), sin(angle)) * pow(radius, 0.75);
        radius += radius_step;
        angle += ANGLE_STEP;
    }
}

float find_blocker(sampler2D shadow_map, vec2 uv, float z_receiver) {
    // calculate average blocker depth
    poisson_disk_samples(uv);

    float avg_blocker_depth = 0.0;
    float blocker_depth = 0.0;
    float blocker_count = 0.0;

    vec2 shadow_map_size = textureSize(shadow_map, 0);
    float filter_size = 10.0;

    for (int i = 0; i < NUM_SAMPLES; i++) {
        vec2 shadow_coord =  uv.xy + poisson_disk[i] * filter_size / shadow_map_size;
        vec4 rgba  = texture2D(shadow_map, shadow_coord).rgba;
        blocker_depth = unpack(rgba);

        if (blocker_depth + 0.001 < z_receiver) {
            avg_blocker_depth += blocker_depth;
            blocker_count += 1.0;
        }
    }

    if (blocker_count == 0.0) return 1.0;
    return avg_blocker_depth / blocker_count;
}

// Hard Shadow Mapping.
float shadow_mapping(sampler2D shadow_map, vec3 shadow_coord)
{
    float current_depth = shadow_coord.z;
    float shadow_depth = texture2D(shadow_map, shadow_coord.xy).r;

    float visibility = shadow_depth + EPS > current_depth ? 1.0 : 0.0;
    return visibility;
}

// Percentage-closer filtering (PCF).
float PCF(sampler2D shadow_map, vec3 shadow_coord, float filter_size)
{
    poisson_disk_samples(shadow_coord.xy);

    float shadow_depth = 0.0;
    float current_depth = shadow_coord.z;
    vec2 shadow_map_size = textureSize(shadow_map, 0);

    float visibility = 0.0;
    for (int i = 0; i < NUM_SAMPLES; i++) {
        vec2 uv =  shadow_coord.xy + poisson_disk[i] * filter_size / shadow_map_size;
        float shadow_depth = texture2D(shadow_map, uv).r;

        visibility += shadow_depth + EPS > current_depth ? 1.0 : 0.0;
    }

    visibility /= float(NUM_SAMPLES);
    return visibility;
}

// Percentage closer soft shadows (PCSS).
float PCSS(sampler2D shadow_map, vec3 shadow_coord)
{
    // Calculate average blocker depth.
    float receiver_depth = shadow_coord.z;
    float blocker_depth = find_blocker(shadow_map, shadow_coord.xy, receiver_depth);

    // Calculate penumbra size.
    float penumbra = (receiver_depth - blocker_depth) * LIGHT_WIDTH / blocker_depth;
    if (penumbra < 0.0) {
        penumbra = 5.0;
    }

    // Do PCF filtering,
    float visibility = PCF(shadow_map, shadow_coord, penumbra);
    return visibility;
}

float evaluate_directional_shadow(int index, vec3 world_pos)
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
    }

    return visibility;
}

float evaluate_spot_shadow(int index, vec3 world_pos)
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
    }

    return visibility;
}
