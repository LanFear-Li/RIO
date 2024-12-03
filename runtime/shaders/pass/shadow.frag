uniform bool use_vsm;

out vec4 fragColor;

void main()
{
    if (use_vsm) {
        float depth = gl_FragCoord.z;

        // VSM shadow map: R for depth, G for square depth.
        fragColor = vec4(depth, depth * depth, 0.0, 0.0);
    }

    gl_FragDepth = gl_FragCoord.z;
}
