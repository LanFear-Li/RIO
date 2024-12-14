#define SMAA_GLSL_4 1
#define SMAA_PRESET_ULTRA 1

#include "../post/smaa.glsl"

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 texCoord;
out vec2 pixCoord;
out vec4 offset[3];

void main()
{
    texCoord = aTexCoord;
    pixCoord = aTexCoord * SMAA_RT_METRICS.zw;

    // We will use these offsets for the searches later on (see @PSEUDO_GATHER4):
    offset[0] = mad(SMAA_RT_METRICS.xyxy, vec4(-0.25, -0.125, 1.25, -0.125), aTexCoord.xyxy);
    offset[1] = mad(SMAA_RT_METRICS.xyxy, vec4(-0.125, -0.25, -0.125, 1.25), aTexCoord.xyxy);

    // And these for the searches, they indicate the ends of the loops:
    offset[2] = mad(
        SMAA_RT_METRICS.xxyy,
        vec4(-2.0, 2.0, -2.0, 2.0) * float(SMAA_MAX_SEARCH_STEPS),
        vec4(offset[0].xz, offset[1].yw)
    );

    gl_Position = vec4(aPos.xy, 0.0, 1.0);
}
