#include "mesh.hpp"

#include <glad/glad.h>

#include <iostream>

static float cube_vertices[] = {
    // back face
    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
     1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
     1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 0.0f, // bottom-right
     1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 1.0f, 1.0f, // top-right
    -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 0.0f, // bottom-left
    -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, -1.0f, 0.0f, 1.0f, // top-left
    // front face
    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
     1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 0.0f, // bottom-right
     1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
     1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f, 1.0f, // top-right
    -1.0f,  1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 1.0f, // top-left
    -1.0f, -1.0f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f, 0.0f, // bottom-left
    // left face
    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
    -1.0f,  1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-left
    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
    -1.0f, -1.0f, -1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-left
    -1.0f, -1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-right
    -1.0f,  1.0f,  1.0f, -1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-right
    // right face
     1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
     1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
     1.0f,  1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 1.0f, // top-right
     1.0f, -1.0f, -1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 1.0f, // bottom-right
     1.0f,  1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 1.0f, 0.0f, // top-left
     1.0f, -1.0f,  1.0f,  1.0f,  0.0f,  0.0f, 0.0f, 0.0f, // bottom-left
    // bottom face
    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
     1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 1.0f, // top-left
     1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
     1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 1.0f, 0.0f, // bottom-left
    -1.0f, -1.0f,  1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 0.0f, // bottom-right
    -1.0f, -1.0f, -1.0f,  0.0f, -1.0f,  0.0f, 0.0f, 1.0f, // top-right
    // top face
    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
     1.0f,  1.0f , 1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
     1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 1.0f, // top-right
     1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 1.0f, 0.0f, // bottom-right
    -1.0f,  1.0f, -1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 1.0f, // top-left
    -1.0f,  1.0f,  1.0f,  0.0f,  1.0f,  0.0f, 0.0f, 0.0f  // bottom-left
    };

static float quad_vertices[] = {
    // Positions, Texture Coords
    -1.0f,  1.0f, 0.0f, 0.0f, 1.0f,
    -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
     1.0f,  1.0f, 0.0f, 1.0f, 1.0f,
     1.0f, -1.0f, 0.0f, 1.0f, 0.0f,
};

Mesh::Mesh(Mesh_Type type)
{
    if (type == Mesh_Type::MESH_CUBE) {
        setup_cube_mesh();
    } else if (type == Mesh_Type::MESH_QUAD) {
        setup_quad_mesh();
    } else {
        std::cerr << "Invalid mesh type" << std::endl;
    }
}

Mesh::Mesh(std::vector<Vertex> vertices, std::vector<unsigned int> indices, unsigned int index)
    : vertices(vertices), indices(indices), material_index(index)
{
    setup_mesh();
}

void Mesh::setup_mesh()
{
    // Create buffers / arrays.
    vertex_array = std::make_unique<Vertex_Array>();
    vertex_buffer = std::make_unique<Vertex_Buffer>(&vertices[0], vertices.size() * sizeof(Vertex));
    index_buffer = std::make_unique<Index_Buffer>(&indices[0], indices.size() * sizeof(unsigned int));

    // Set the vertex attribute pointers.
    vertex_attribute = std::make_unique<Vertex_Attribute>(sizeof(Vertex));
    vertex_attribute->add_attribute(Vertex_Data_Type::FLOAT, 3, 0);
    vertex_attribute->add_attribute(Vertex_Data_Type::FLOAT, 3, offsetof(Vertex, normal));
    vertex_attribute->add_attribute(Vertex_Data_Type::FLOAT, 2, offsetof(Vertex, tex_coord));
    vertex_attribute->add_attribute(Vertex_Data_Type::FLOAT, 3, offsetof(Vertex, tangent));
    vertex_attribute->add_attribute(Vertex_Data_Type::FLOAT, 3, offsetof(Vertex, bitangent));
    vertex_attribute->add_attribute(Vertex_Data_Type::INT, 4, offsetof(Vertex, m_bone_ids));
    vertex_attribute->add_attribute(Vertex_Data_Type::FLOAT, 4, offsetof(Vertex, m_weights));

    vertex_array->unbind();
    vertex_buffer->unbind();
    index_buffer->unbind();
}

void Mesh::setup_cube_mesh()
{
    vertex_array = std::make_unique<Vertex_Array>();
    vertex_buffer = std::make_unique<Vertex_Buffer>(&cube_vertices, sizeof(cube_vertices));

    vertex_attribute = std::make_unique<Vertex_Attribute>(8 * sizeof(float));
    vertex_attribute->add_attribute(Vertex_Data_Type::FLOAT, 3, 0);
    vertex_attribute->add_attribute(Vertex_Data_Type::FLOAT, 3, (3 * sizeof(float)));
    vertex_attribute->add_attribute(Vertex_Data_Type::FLOAT, 2, (6 * sizeof(float)));

    vertex_array->unbind();
    vertex_buffer->unbind();
}

void Mesh::setup_quad_mesh()
{
    vertex_array = std::make_unique<Vertex_Array>();
    vertex_buffer = std::make_unique<Vertex_Buffer>(&quad_vertices, sizeof(quad_vertices));

    vertex_attribute = std::make_unique<Vertex_Attribute>(5 * sizeof(float));
    vertex_attribute->add_attribute(Vertex_Data_Type::FLOAT, 3, 0);
    vertex_attribute->add_attribute(Vertex_Data_Type::FLOAT, 2, (3 * sizeof(float)));

    vertex_array->unbind();
    vertex_buffer->unbind();
}
