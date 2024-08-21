#include "../model/blinn-phong.glsl"
#include "../model/brdf.glsl"

out vec4 fragColor;

in vec2 texCoord;
in vec3 worldPos;
in vec3 normal;

uniform vec3 lightColor;

void main()
{
    vec3 result = lightColor;

    // Gamma correction.
    result = pow(result, vec3(1.0 / 2.2));

    fragColor = vec4(result, 1.0);
}
