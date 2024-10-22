#pragma once

#include "marcos.glsl"

float saturate(float x)
{
    return clamp(x, 0.0, 1.0);
}

float pow2(float x)
{
    return x * x;
}

float pow4(float x)
{
    x = pow2(x);
    return x * x;
}

highp float rand_2to1(vec2 uv)
{
	const highp float a = 12.9898, b = 78.233, c = 43758.5453;
	highp float dt = dot( uv.xy, vec2( a,b ) ), sn = mod( dt, PI );
	return fract(sin(sn) * c);
}

float unpack(vec4 rgba_depth)
{
    const vec4 bit_shift = vec4(1.0, 1.0 / 256.0, 1.0 / (256.0 * 256.0), 1.0 / (256.0 * 256.0 * 256.0));
    return dot(rgba_depth, bit_shift);
}
