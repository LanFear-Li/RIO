#include "../post/fxaa.glsl"

in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D shade_color;

void main()
{
    vec2 size = textureSize(shade_color, 0);

    vec4 color = compute_fxaa(shade_color, texCoord, size);

    fragColor = color;
}
