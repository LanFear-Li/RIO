#pragma once

#include "platform/vertex-array.hpp"
#include "platform/vertex-buffer.hpp"
#include "platform/index-buffer.hpp"
#include "platform/vertex-attribute.hpp"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>
#include <memory>

#define MAX_BONE_INFLUENCE 4

struct Vertex
{
    glm::vec3 position;
    glm::vec3 normal;
    glm::vec2 tex_coord;
    glm::vec3 tangent;
    glm::vec3 bitangent;

    // Bone indexes which will influence this vertex.
    int m_bone_ids[MAX_BONE_INFLUENCE];
    // Weights from each bone.
    float m_weights[MAX_BONE_INFLUENCE];
};

enum Mesh_Type
{
    MESH_NORMAL,
    MESH_CUBE,
    MESH_QUAD
};

class Mesh final
{
public:
    Mesh() = delete;
    Mesh(Mesh_Type type);
    Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, unsigned int index);

    // Mesh data storage.
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    unsigned int material_index;

    // Vertex array object storage.
    std::unique_ptr<Vertex_Array> vertex_array;

private:
    // Initializes all the buffer objects / arrays.
    void setup_mesh();
    void setup_cube_mesh();
    void setup_quad_mesh();

    // Vertex buffer and index buffer storage.
    std::unique_ptr<Vertex_Buffer> vertex_buffer;
    std::unique_ptr<Index_Buffer> index_buffer;
    std::unique_ptr<Vertex_Attribute> vertex_attribute;
};
