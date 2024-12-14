#define SMAA_GLSL_4 1
#define SMAA_PRESET_ULTRA 1

#include "../post/smaa.glsl"

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTexCoord;

out vec2 texCoord;
out vec4 offset[3];

void main()
{
    texCoord = aTexCoord;

    offset[0] = mad(SMAA_RT_METRICS.xyxy, vec4(-1.0, 0.0, 0.0, -1.0), aTexCoord.xyxy);
    offset[1] = mad(SMAA_RT_METRICS.xyxy, vec4( 1.0, 0.0, 0.0,  1.0), aTexCoord.xyxy);
    offset[2] = mad(SMAA_RT_METRICS.xyxy, vec4(-2.0, 0.0, 0.0, -2.0), aTexCoord.xyxy);

    gl_Position = vec4(aPos.xy, 0.0, 1.0);
}
