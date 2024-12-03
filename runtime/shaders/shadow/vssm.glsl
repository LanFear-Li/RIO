#pragma once

#include "../common/parameter.glsl"
#include "../util/math.glsl"

vec2 sample_SAT(sampler2D SAT_map, vec3 shadow_coord, float filter_size)
{
    vec2 shadow_map_size = textureSize(SAT_map, 0);
    vec2 stride = 1.0 / shadow_map_size;

	float xmax = shadow_coord.x + filter_size * stride.x;
	float xmin = shadow_coord.x - filter_size * stride.x;
	float ymax = shadow_coord.y + filter_size * stride.y;
	float ymin = shadow_coord.y - filter_size * stride.y;

	vec2 A = texture(SAT_map, vec2(xmin, ymin)).xy;
	vec2 B = texture(SAT_map, vec2(xmax, ymin)).xy;
	vec2 C = texture(SAT_map, vec2(xmin, ymax)).xy;
	vec2 D = texture(SAT_map, vec2(xmax, ymax)).xy;

	float len_side = 2.0 * filter_size;

	vec2 moments = (D + A - B - C) / float(len_side * len_side);
	return moments;
}

// Chebyshev's inequality: used to estimate CDF (percentage of non-blockers).
float chebyshev(vec2 moments, float current_depth)
{
    float mean = moments.x;

    // No occluder in shadow map.
    if (current_depth <= mean) {
		return 1.0;
	}

    // Calculate variance: Var(x) = E(x^2) - E(x)^2.
    float variance = max(moments.y - (mean * mean), 0.0001);

    // Use Chebyshev's inequality to calculate probability.
	float d = current_depth - mean;
	float p_max = variance / (variance + d * d);
	return p_max;
}

// Variance Shadow Mapping (VSM).
float VSM(sampler2D shadow_map, sampler2D SAT_map, vec3 shadow_coord, float filter_size)
{
    vec2 moments = sample_SAT(SAT_map, shadow_coord, filter_size);
    float current_depth = shadow_coord.z;

    if (current_depth <= moments.x) {
		return 1.0;
	}

    float visibility = chebyshev(moments, current_depth);
    return visibility;
}

// Variance Soft Shadow Mapping (VSSM).
float VSSM(sampler2D shadow_map, sampler2D SAT_map, vec3 shadow_coord)
{
    // Estimate average blocker depth.
    float current_depth = shadow_coord.z;
    float clostest_depth = texture(shadow_map, shadow_coord.xy).r;
    float blocker_search_size = LIGHT_WIDTH;

    // Cut out the no padding area according to the sarched area size.
    float border = blocker_search_size / textureSize(shadow_map, 0).x;
	if (shadow_coord.x <= border || shadow_coord.x >= 0.99 - border){
		return 1.0;
	}
	if (shadow_coord.y <= border || shadow_coord.y >= 0.99 - border) {
		return 1.0;
	}

    vec2 moments = sample_SAT(SAT_map, shadow_coord, blocker_search_size);
    float average_blocker_depth = moments.x;

    float alpha = chebyshev(moments, current_depth);
    float blocker_depth = (average_blocker_depth - alpha * (current_depth)) / (1.0 - alpha);

    // if (blocker_depth < EPS) {
	// 	return 0.0;
	// }
	if (blocker_depth > 1.0) {
		return 1.0;
	}

    // Calculate penumbra size.
    float penumbra = (current_depth - blocker_depth) * LIGHT_WIDTH / blocker_depth;
    if (penumbra < 0.0) {
        penumbra = 5.0;
    }

    // Do VSM estimation.
    float visibility = VSM(shadow_map, SAT_map, shadow_coord, penumbra);
    return visibility;
}
