#include "../model/blinn-phong.glsl"
#include "../model/brdf.glsl"

out vec4 fragColor;

in vec2 texCoord;
in vec3 worldPos;
in vec3 normal;

void main()
{
    Material material = init_material();
    collect_material(texCoord, material);

    if (use_normal_map == false) {
        material.normal = normal;
    }

    vec3 result = vec3(0.0);
    if (shading_model == PHONG) {
        result = evaluate_phong(worldPos, eyePos, material);
    } else if (shading_model == BLINN_PHONG) {
        result = evaluate_blinn_phong(worldPos, eyePos, material);
    } else if (shading_model == BRDF) {
        result = evaluate_brdf(worldPos, eyePos, material);
    }

    // Gamma correction.
    result = pow(result, vec3(1.0 / 2.2));

    fragColor = vec4(result, 1.0);
}
