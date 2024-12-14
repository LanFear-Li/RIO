#define SMAA_GLSL_4 1
#define SMAA_PRESET_ULTRA 1

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
    vec3 c = texture(colorTex, texCoord).rgb;

    // Left, Top, Right, Bottom, Left Left, Top Top.
    delta[0] = max3(abs(c - texture(colorTex, offset[0].xy).rgb));
    delta[1] = max3(abs(c - texture(colorTex, offset[0].zw).rgb));
    delta[2] = max3(abs(c - texture(colorTex, offset[1].xy).rgb));
    delta[3] = max3(abs(c - texture(colorTex, offset[1].zw).rgb));
    delta[4] = max3(abs(c - texture(colorTex, offset[2].xy).rgb));
    delta[5] = max3(abs(c - texture(colorTex, offset[2].zw).rgb));

    // We do the usual threshold:
    vec2 edges;
    edges.x = step(threshold, delta[0]);
    edges.y = step(threshold, delta[1]);

    // Then discard if there is no edge:
    if (dot(edges, vec2(1.0, 1.0)) == 0.0) {
        discard;
    }

    // Calculate the final maximum delta.
    float mask_delta_max = max(max(delta[2], delta[3]), max(delta[4], delta[5]));
    float left_delta_max = max(mask_delta_max, delta[0]);
    float top_delta_max = max(mask_delta_max, delta[1]);
    float final_delta = max(left_delta_max, top_delta_max);

    // Local contrast adaptation.
    edges.x *= step(final_delta, SMAA_LOCAL_CONTRAST_ADAPTATION_FACTOR * delta[0]);
    edges.y *= step(final_delta, SMAA_LOCAL_CONTRAST_ADAPTATION_FACTOR * delta[1]);

    edge_detection = vec4(edges, 0.0, 1.0);
}
