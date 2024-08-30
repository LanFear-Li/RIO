#include "../util/evaluate.glsl"

in vec2 texCoord;
out vec2 fragColor;

vec2 integrate_brdf(float cos_N_V, float roughness)
{
    vec3 V;
    V.x = sqrt(1.0 - cos_N_V * cos_N_V);
    V.y = 0.0;
    V.z = cos_N_V;

    float A = 0.0;
    float B = 0.0;

    vec3 N = vec3(0.0, 0.0, 1.0);

    const uint SAMPLE_COUNT = 1024u;
    for(uint i = 0u; i < SAMPLE_COUNT; ++i) {
        // generates a sample vector that's biased towards the
        // preferred alignment direction (importance sampling).
        vec2 Xi = Hammersley(i, SAMPLE_COUNT);
        vec3 H = importance_sample_GGX(roughness, Xi, N);
        vec3 L = normalize(2.0 * dot(V, H) * H - V);

        float NdotL = max(L.z, 0.0);
        float NdotH = max(H.z, 0.0);

        float cos_V_H = max(dot(V, H), 0.0);
        float cos_N_L = max(dot(N, L), 0.0);
        float cos_N_V = max(dot(N, V), 0.0);

        if(NdotL > 0.0) {
            float G = visibility_Smith(roughness, cos_N_V, cos_N_L);
            float G_Vis = (G * cos_V_H) / (NdotH * cos_N_V);
            float Fc = pow(1.0 - cos_V_H, 5.0);

            A += (1.0 - Fc) * G_Vis;
            B += Fc * G_Vis;
        }
    }

    A /= float(SAMPLE_COUNT);
    B /= float(SAMPLE_COUNT);
    return vec2(A, B);
}

void main()
{
    vec2 integrated_brdf = integrate_brdf(texCoord.x, texCoord.y);
    fragColor = integrated_brdf;
}
