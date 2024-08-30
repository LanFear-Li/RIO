#include "../common/parameter.glsl"

in vec3 texCoord;
out vec4 fragColor;

void main()
{
    vec3 skyboxCcolor = texture(environment_map, texCoord).rgb;
    fragColor = vec4(skyboxCcolor, 1.0);
}
