#define SMAA_GLSL_4 1

#include "../post/smaa.glsl"

in vec2 texCoord;
in vec4 offset;

out vec4 neighborhood_blending;

void main()
{
    vec4 color;

    // Fetch the blending weights for current pixel:
    vec4 a;
    a.x = texture2D(blendTex, offset.xy).a; // Right
    a.y = texture2D(blendTex, offset.zw).g; // Top
    a.wz = texture2D(blendTex, texCoord).xz; // Bottom / Left

    // Is there any blending weight with a value greater than 0.0?
    if (dot(a, vec4(1.0, 1.0, 1.0, 1.0)) <= 1e-5) {
        color = texture2D(blendTex, texCoord); // LinearSampler
    } else {
        bool h = max(a.x, a.z) > max(a.y, a.w); // max(horizontal) > max(vertical)

        // Calculate the blending offsets:
        vec4 blendingOffset = vec4(0.0, a.y, 0.0, a.w);
        vec2 blendingWeight = a.yw;
        SMAAMovc(bvec4(h, h, h, h), blendingOffset, vec4(a.x, 0.0, a.z, 0.0));
        SMAAMovc(bvec2(h, h), blendingWeight, a.xz);
        blendingWeight /= dot(blendingWeight, vec2(1.0, 1.0));

        // Calculate the texture coordinates:
        vec4 blendingCoord = mad(blendingOffset, vec4(SMAA_RT_METRICS.xy, -SMAA_RT_METRICS.xy), texCoord.xyxy);

        // We exploit bilinear filtering to mix current pixel with the chosen
        // neighbor:
        color = blendingWeight.x * texture2D(blendTex, blendingCoord.xy); // LinearSampler
        color += blendingWeight.y * texture2D(blendTex, blendingCoord.zw); // LinearSampler
    }

    neighborhood_blending = color;
}
