#pragma once

uniform sampler2D color_current;
uniform sampler2D depth_current;
uniform sampler2D color_previous;
uniform sampler2D velocity_tex;

uniform int buffer_width;
uniform int buffer_height;

vec2 search_closest_offset(vec2 tex_coord)
{
    vec2 delta_res = vec2(1.0 / buffer_width, 1.0 / buffer_height);

    float closest_depth = 1.0f;
    vec2 closest_uv = tex_coord;

    // Search offset of closest depth in 3x3 grid.
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            vec2 new_uv = tex_coord + delta_res * vec2(i, j);
            float depth = texture2D(depth_current, new_uv).x;

            if(depth < closest_depth) {
                closest_depth = depth;
                closest_uv = new_uv;
            }
        }
    }

    return closest_uv;
}

vec3 RGB_2_YCoCg(vec3 color_rgb)
{
    vec3 color_YCoCg;

    color_YCoCg.y = color_rgb.r - color_rgb.b;
    float temp = color_rgb.b + color_YCoCg.y / 2;
    color_YCoCg.z = color_rgb.g - temp;
    color_YCoCg.x = temp + color_YCoCg.z / 2;

    return color_YCoCg;
}

vec3 YCoCg_2_RGB(vec3 color_YCoCg)
{
    vec3 color_rgb;

    float temp = color_YCoCg.x - color_YCoCg.z / 2;
    color_rgb.g = color_YCoCg.z + temp;
    color_rgb.b = temp - color_YCoCg.y / 2;
    color_rgb.r = color_rgb.b + color_YCoCg.y;

    return color_rgb;
}

float luminance(vec3 color)
{
    return 0.25 * color.r + 0.5 * color.g + 0.25 * color.b;
}

vec3 tone_mapping(vec3 color)
{
    return color / (1 + luminance(color));
}

vec3 un_tone_mapping(vec3 color)
{
    return color / (1 - luminance(color));
}

vec3 clip_AABB(vec3 color_cur, vec3 color_prev, vec2 tex_coord)
{
    vec3 aabb_min = color_cur, aabb_max = color_cur;
    vec2 delta_res = vec2(1.0 / buffer_width, 1.0 / buffer_height);
    vec3 m1 = vec3(0), m2 = vec3(0);

    // Calculate RGB AABB of 3x3 grid.
    for (int i = -1; i <= 1; ++i) {
        for (int j = -1; j <= 1; ++j) {
            vec2 new_uv = tex_coord + delta_res * vec2(i, j);
            vec3 C = RGB_2_YCoCg(tone_mapping(texture(color_current, new_uv).rgb));
            m1 += C;
            m2 += C * C;
        }
    }

    // Variance clip.
    const int N = 9;
    const float variance_clip_gamma = 1.0f;
    vec3 mu = m1 / N;
    vec3 sigma = sqrt(abs(m2 / N - mu * mu));
    aabb_min = mu - variance_clip_gamma * sigma;
    aabb_max = mu + variance_clip_gamma * sigma;

    // Clip to center.
    vec3 p_clip = 0.5 * (aabb_max + aabb_min);
    vec3 e_clip = 0.5 * (aabb_max - aabb_min);

    vec3 v_clip = color_prev - p_clip;
    vec3 v_unit = v_clip.xyz / e_clip;
    vec3 a_unit = abs(v_unit);
    float ma_unit = max(a_unit.x, max(a_unit.y, a_unit.z));

    if (ma_unit > 1.0) {
        return p_clip + v_clip / ma_unit;
    } else {
        return color_prev;
    }
}

vec3 compute_taa(vec3 color_cur, vec2 tex_coord)
{
    vec2 closest_uv = search_closest_offset(tex_coord);
    vec2 velocity = texture(velocity_tex, closest_uv).rg;
    vec2 offset_uv = clamp(tex_coord - velocity, vec2(0.0), vec2(1.0));
    vec3 color_prev = texture(color_previous, tex_coord).rgb;

    color_cur = RGB_2_YCoCg(tone_mapping(color_cur));
    color_prev = RGB_2_YCoCg(tone_mapping(color_prev));

    color_prev = clip_AABB(color_cur, color_prev, tex_coord);

    color_prev = un_tone_mapping(YCoCg_2_RGB(color_prev));
    color_cur = un_tone_mapping(YCoCg_2_RGB(color_cur));

    // Blend between current and previous frame with weight.
    float mix_coff = 0.05;
    vec3 color_mix = mix(color_prev, color_cur, mix_coff);

    return color_mix;
}
