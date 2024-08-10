#include "model/blinn-phong.glsl"

out vec4 fragColor;

in vec2 texCoord;
in vec3 worldPos;
in vec3 normal;

void main()
{
    Material material = init_material();
    collect_material(texCoord, material);

    material.normal = normal;

    vec3 result = vec3(0.0);
    // result = evaluate_phong(worldPos, eyePos, material);
    result = evaluate_blinn_phong(worldPos, eyePos, material);

    fragColor = vec4(result, 1.0);
}
