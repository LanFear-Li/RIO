layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;
layout (location = 2) in vec2 aTexCoord;
layout (location = 3) in vec3 aTangent;
layout (location = 4) in vec3 aBitangent;

out vec2 texCoord;
out vec3 worldPos;
out vec3 normal;
out vec3 tangent;
out vec3 bitangent;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main()
{
    texCoord = aTexCoord;
    worldPos = (model * vec4(aPos, 1.0)).xyz;
    normal = mat3(transpose(inverse(model))) * aNormal;
    tangent = mat3(transpose(inverse(model))) * aTangent;
    bitangent = mat3(transpose(inverse(model))) * aBitangent;

    gl_Position = projection * view * model * vec4(aPos, 1.0f);
}
