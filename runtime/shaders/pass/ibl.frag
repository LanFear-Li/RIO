out vec4 fragColor;

in vec3 texCoord;

uniform samplerCube _texture_ibl;

void main()
{
    fragColor = texture(_texture_ibl, texCoord);
}
