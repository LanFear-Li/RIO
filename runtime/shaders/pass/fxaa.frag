#include "../post/fxaa.glsl"

in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D input_fxaa;

void main()
{
    vec2 size = textureSize(input_fxaa, 0);

    vec4 color = compute_fxaa(input_fxaa, texCoord, size);

    fragColor = color;
}
