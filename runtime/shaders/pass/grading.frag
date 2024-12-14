in vec2 texCoord;
out vec4 fragColor;

uniform sampler2D input_grading;

void main()
{
    vec3 color = texture(input_grading, texCoord).rgb;

    // Gamma correction.
    // Linear Space -> sRGB.
    color = pow(color, vec3(1.0 / 2.2));

    fragColor = vec4(color, 1.0);
}
