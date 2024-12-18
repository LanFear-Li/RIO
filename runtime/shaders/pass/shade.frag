#include "../model/blinn-phong.glsl"
#include "../model/brdf.glsl"

layout(location = 0) out vec4 fragColor;
layout(location = 1) out vec2 velocity;

in vec2 texCoord;
in vec3 worldPos;
in vec3 normal;
in vec3 tangent;
in vec3 bitangent;

in vec4 prev_clip_pos;
in vec4 cur_clip_pos;

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
    if (shading_method == PHONG) {
        result = evaluate_phong(worldPos, eyePos, material);
    } else if (shading_method == BLINN_PHONG) {
        result = evaluate_blinn_phong(worldPos, eyePos, material);
    } else if (shading_method == BRDF) {
        result = evaluate_brdf(worldPos, eyePos, material);
    }

    fragColor = vec4(result, 1.0);

    vec2 prev_pos = ((prev_clip_pos.xy / prev_clip_pos.w) * 0.5 + 0.5);
    vec2 cur_pos = ((cur_clip_pos.xy / cur_clip_pos.w) * 0.5 + 0.5);
    velocity = cur_pos - prev_pos;
}
