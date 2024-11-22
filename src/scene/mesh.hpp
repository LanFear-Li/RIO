#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <string>
#include <vector>

using namespace std;

#define MAX_BONE_INFLUENCE 4

struct Vertex
{
    // position
    glm::vec3 Position;
    // normal
    glm::vec3 Normal;
    // texCoords
    glm::vec2 TexCoords;
    // tangent
    glm::vec3 Tangent;
    // bitangent
    glm::vec3 Bitangent;
    //bone indexes which will influence this vertex
    int m_BoneIDs[MAX_BONE_INFLUENCE];
    //weights from each bone
    float m_Weights[MAX_BONE_INFLUENCE];
};

class Mesh final
{
public:
    Mesh() = default;
    Mesh(vector<Vertex> vertices, vector<unsigned int> indices, unsigned int index);

    void setupCubeMesh();
    void setupQuadMesh();

    // Mesh data storage.
    vector<Vertex> vertices;
    vector<unsigned int> indices;
    unsigned int materialIndex;
    unsigned int VAO{};

private:
    // Initializes all the buffer objects / arrays.
    void setupMesh();

    // Render data storage.
    unsigned int VBO{};
    unsigned int EBO{};
};
