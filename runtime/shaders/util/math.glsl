#pragma once

#define PI          3.141592653589793
#define INV_PI      0.3183098861837907
#define HALF_PI     1.5707963267948966

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
