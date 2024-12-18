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

out vec4 prev_clip_pos;
out vec4 cur_clip_pos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;
uniform mat4 prev_mvp_matrix;

uniform bool enable_taa;
uniform int buffer_width;
uniform int buffer_height;
uniform int frame_count;

const vec2 Halton_2_3[8] =
{
    vec2( 0.0 / 1.0, -1.0 / 3.0),
    vec2(-1.0 / 2.0,  1.0 / 3.0),
    vec2( 1.0 / 2.0, -7.0 / 9.0),
    vec2(-3.0 / 4.0, -1.0 / 9.0),
    vec2( 1.0 / 4.0,  5.0 / 9.0),
    vec2(-1.0 / 4.0, -5.0 / 9.0),
    vec2( 3.0 / 4.0,  1.0 / 9.0),
    vec2(-7.0 / 8.0,  7.0 / 9.0)
};

void main()
{
    texCoord = aTexCoord;
    worldPos = (model * vec4(aPos, 1.0)).xyz;
    normal = mat3(transpose(inverse(model))) * aNormal;
    tangent = mat3(transpose(inverse(model))) * aTangent;
    bitangent = mat3(transpose(inverse(model))) * aBitangent;

    mat4 modified_projection = projection;
    if (enable_taa) {
        float delta_width = 1.0 / buffer_width, delta_height = 1.0 / buffer_height;
        int offset_idx = frame_count % 8;
        vec2 jitter = vec2(
            Halton_2_3[offset_idx].x * delta_width,
            Halton_2_3[offset_idx].y * delta_height
        );

        modified_projection[2][0] += jitter.x;
        modified_projection[2][1] += jitter.y;
    }

    prev_clip_pos = (prev_mvp_matrix * vec4(aPos, 1.0));
    cur_clip_pos = (projection * view * model * vec4(aPos, 1.0));

    gl_Position = modified_projection * view * model * vec4(aPos, 1.0);
}
