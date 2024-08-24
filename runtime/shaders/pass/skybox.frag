out vec4 fragColor;

in vec3 texCoord;

uniform samplerCube _texture_ibl;

uniform bool from_equirectangular;

void main()
{
    vec3 skyboxCcolor = texture(_texture_ibl, texCoord).rgb;

    if (from_equirectangular == true) {
        // HDR Tonemap and gamma correct.
        skyboxCcolor = skyboxCcolor / (skyboxCcolor + vec3(1.0));
        skyboxCcolor = pow(skyboxCcolor, vec3(1.0 / 2.2));
    }

    fragColor = vec4(skyboxCcolor, 1.0);
}
