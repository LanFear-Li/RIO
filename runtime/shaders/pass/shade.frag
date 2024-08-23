#include "../model/blinn-phong.glsl"
#include "../model/brdf.glsl"

out vec4 fragColor;

in vec2 texCoord;
in vec3 worldPos;
in vec3 normal;
in vec3 tangent;
in vec3 bitangent;

void main()
{
    Material material = init_material();
    collect_material(texCoord, material);

    // Retrive normal to world space.
    if (use_normal_map == true) {
        vec3 T = normalize(tangent);
        vec3 B = normalize(bitangent);
        vec3 N = normalize(normal);
        mat3 TBN = mat3(T, B, N);

        material.normal = normalize(TBN * material.normal);
    } else {
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
