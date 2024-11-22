#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

using namespace std;

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

class Mesh final
{
public:
    Mesh() = default;
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, unsigned int index);

    void setup_cube_mesh();
    void setup_quad_mesh();

    // Mesh data storage.
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    unsigned int material_index;
    unsigned int VAO{};

private:
    // Initializes all the buffer objects / arrays.
    void setup_mesh();

    // Render data storage.
    unsigned int VBO{};
    unsigned int EBO{};
};
