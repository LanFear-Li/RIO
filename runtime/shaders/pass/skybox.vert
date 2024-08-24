layout (location = 0) in vec3 aPos;

out vec3 texCoord;

uniform mat4 view;
uniform mat4 projection;

void main()
{
    texCoord = aPos;

    // Extract camera rotation from view matrix.
    mat4 view_rotate = mat4(mat3(view));
    vec4 pos = projection * view_rotate * vec4(aPos, 1.0);
    gl_Position = pos.xyww;
}
