#include "../common/parameter.glsl"

in vec3 texCoord;
out vec4 fragColor;

void main()
{
    vec3 skybox_color = texture(environment_map, texCoord).rgb;

    // Linear Space <- sRGB.
    skybox_color = pow(skybox_color, vec3(2.2));

    fragColor = vec4(skybox_color, 1.0);
}
