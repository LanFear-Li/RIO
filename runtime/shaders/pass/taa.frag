#include "../post/taa.glsl"

in vec2 texCoord;
out vec4 fragColor;

uniform int frame_count;

void main()
{
    vec3 color_cur = texture(color_current, texCoord).rgb;

    if (frame_count == 0) {
        fragColor = vec4(color_cur, 1.0);
        return;
    }

    vec3 color_mix = compute_taa(color_cur, texCoord);

    fragColor = vec4(color_mix, 1.0);
}
