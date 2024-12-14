#define SMAA_GLSL_4 1

#include "../post/smaa.glsl"

in vec2 texCoord;
in vec4 offset[3];

out vec4 edge_detection;

float max3(vec3 c)
{
    return max(c.x, max(c.y, c.z));
}

void main()
{
    float threshold = SMAA_THRESHOLD;

    // Calculate color deltas:
    float delta[6];
    vec3 c = pow(texture(colorTex, texCoord).rgb, vec3(GAMMA_FOR_EDGE_DETECTION));

    delta[0] = max3(abs(c - pow(texture(colorTex, offset[0].xy).rgb, vec3(GAMMA_FOR_EDGE_DETECTION))));
    delta[1] = max3(abs(c - pow(texture(colorTex, offset[0].zw).rgb, vec3(GAMMA_FOR_EDGE_DETECTION))));
    delta[2] = max3(abs(c - pow(texture(colorTex, offset[1].xy).rgb, vec3(GAMMA_FOR_EDGE_DETECTION))));
    delta[3] = max3(abs(c - pow(texture(colorTex, offset[1].zw).rgb, vec3(GAMMA_FOR_EDGE_DETECTION))));
    delta[4] = max3(abs(c - pow(texture(colorTex, offset[2].xy).rgb, vec3(GAMMA_FOR_EDGE_DETECTION))));
    delta[5] = max3(abs(c - pow(texture(colorTex, offset[2].zw).rgb, vec3(GAMMA_FOR_EDGE_DETECTION))));

    float mask_delta_max = max(max(delta[2], delta[3]), max(delta[4], delta[5]));
    float left_delta_max = max(mask_delta_max, delta[0]);
    float top_delta_max = max(mask_delta_max, delta[1]);

    // We do the usual threshold:
    vec2 edges;
    edges.x = step(threshold, delta[0]) * float(delta[0] > 0.5 * left_delta_max);
    edges.y = step(threshold, delta[1]) * float(delta[1] > 0.5 * top_delta_max);

    // Then discard if there is no edge:
    if (dot(edges, vec2(1.0, 1.0)) == 0.0) {
        discard;
    }

    edge_detection = vec4(edges, 0.0, 1.0);
}
