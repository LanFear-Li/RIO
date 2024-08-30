#include "../util/evaluate.glsl"

in vec3 localPos;
out vec4 fragColor;

uniform samplerCube environment_map;

uniform float _ibl_roughness;
uniform float _ibl_resolution;

void main()
{
    vec3 N = normalize(localPos);
    vec3 R = N;
    vec3 V = R;

    const uint SAMPLE_COUNT = 1024u;
    float total_weight = 0.0;
    vec3 prefiltered_color = vec3(0.0);
    for (uint i = 0u; i < SAMPLE_COUNT; ++i) {
        // Generates a sample vector that's biased towards the preferred alignment direction (importance sampling).
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = importance_sample_GGX(_ibl_roughness, Xi, N);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float cos_N_H = max(dot(N, H), 0.0);
        float cos_N_L = max(dot(N, L), 0.0);

        if (cos_N_L > 0.0) {
            // Sample from the environment's mip level based on roughness/pdf.
            float D = distribution_GGX(_ibl_roughness, cos_N_H);
            float NdotH = max(dot(N, H), 0.0);
            float HdotV = max(dot(H, V), 0.0);
            float pdf = D * NdotH / (4.0 * HdotV) + 0.0001;

            // Resolution of source cubemap (per face).
            float resolution = _ibl_resolution;
            float saTexel = 4.0 * PI / (6.0 * resolution * resolution);
            float saSample = 1.0 / (float(SAMPLE_COUNT) * pdf + 0.0001);

            float mipLevel = _ibl_roughness == 0.0 ? 0.0 : 0.5 * log2(saSample / saTexel);

            prefiltered_color += textureLod(environment_map, L, mipLevel).rgb * cos_N_L;
            total_weight += cos_N_L;
        }
    }

    prefiltered_color = prefiltered_color / total_weight;
    fragColor = vec4(prefiltered_color, 1.0);
}
