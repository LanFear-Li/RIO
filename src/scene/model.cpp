#include "model.hpp"

#include "scene/texture.hpp"

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

Model::Model(std::string const &path)
{
    load_model(path);
}

void Model::load_model(std::string const &path)
{
    // read file via ASSIMP
    Assimp::Importer importer;
    const aiScene *scene = importer.ReadFile(path, aiProcess_Triangulate | aiProcess_GenSmoothNormals |
                                                    aiProcess_FlipUVs | aiProcess_CalcTangentSpace);
    // check for errors
    if (!scene || scene->mFlags & AI_SCENE_FLAGS_INCOMPLETE || !scene->mRootNode) // if is Not Zero
    {
        std::cout << "ERROR::ASSIMP:: " << importer.GetErrorString() << std::endl;
        return;
    }
    // retrieve the directory path of the filepath
    directory = path.substr(0, path.find_last_of('/'));

    // process ASSIMP's root node recursively
    process_node(scene->mRootNode, scene);

    // Process each material located at the current node.
    // Assimp will stbi_set_flip_vertically_on_load(true) for you.
    for (unsigned int i = 0; i < scene->mNumMaterials; i++) {
        aiMaterial *material = scene->mMaterials[i];
        materials.push_back(process_material(material, scene));
    }
}

std::unique_ptr<Model> Model::construct_skybox(const std::string &skybox_path, const std::string &skybox_name)
{
    fs::path directoryPath(skybox_path);

    std::error_code ec;
    if (!fs::exists(directoryPath, ec)) {
        std::cerr << "Directory " << directoryPath << " does not exist." << std::endl;
        assert(0);
    }

    int file_count = 0;
    fs::path file_path;
    for (const auto& entry : fs::directory_iterator(directoryPath)) {
        if (fs::is_regular_file(entry.status())) {
            ++file_count;
            file_path = entry;
        }
    }

    if (file_count == 6) {
        // Format cubemap.
        std::vector<std::string> faces {
        skybox_path + "right.jpg",
        skybox_path + "left.jpg",
        skybox_path + "top.jpg",
        skybox_path + "bottom.jpg",
        skybox_path + "front.jpg",
        skybox_path + "back.jpg"
        };

        return Model::construct_cubemap(faces);
    } else if (file_count == 1) {
        // Format equirectangular map.
        std::string hdr_path = skybox_path + skybox_name + file_path.extension().string();
        return Model::construct_equirectangular(hdr_path);
    }

    std::cout << "Initialize skybox failed, check your file format please." << std::endl;
    assert(0);
    return nullptr;
}

std::unique_ptr<Model> Model::construct_equirectangular(const std::string &rect_path)
{
    auto model = std::make_unique<Model>();

    auto material = std::make_unique<Material>();
    material->skybox_map = load_rect_map(rect_path, 3);

    auto mesh = std::make_unique<Mesh>(Mesh_Type::MESH_CUBE);

    model->materials.push_back(std::move(material));
    model->meshes.push_back(std::move(mesh));

    return model;
}

std::unique_ptr<Model> Model::construct_cubemap(std::vector<std::string> cubemap_path)
{
    auto model = std::make_unique<Model>();

    auto material = std::make_unique<Material>();
    material->skybox_map = load_cube_map(cubemap_path, 3);

    auto mesh = std::make_unique<Mesh>(Mesh_Type::MESH_CUBE);

    model->materials.push_back(std::move(material));
    model->meshes.push_back(std::move(mesh));

    return model;
}

std::unique_ptr<Model> Model::construct_cube()
{
    auto model = std::make_unique<Model>();

    auto material = std::make_unique<Material>();
    auto mesh = std::make_unique<Mesh>(Mesh_Type::MESH_CUBE);

    model->materials.push_back(std::move(material));
    model->meshes.push_back(std::move(mesh));

    return model;
}

std::unique_ptr<Model> Model::construct_quad()
{
    auto model = std::make_unique<Model>();

    auto material = std::make_unique<Material>();
    auto mesh = std::make_unique<Mesh>(Mesh_Type::MESH_QUAD);

    model->materials.push_back(std::move(material));
    model->meshes.push_back(std::move(mesh));

    return model;
}

// processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
void Model::process_node(aiNode *node, const aiScene *scene)
{
    // process each mesh located at the current node
    for (unsigned int i = 0; i < node->mNumMeshes; i++) {
        // the node object only contains indices to index the actual objects in the scene.
        // the scene contains all the data, node is just to keep stuff organized (like relations between nodes).
        aiMesh *mesh = scene->mMeshes[node->mMeshes[i]];
        meshes.push_back(process_mesh(mesh, scene));
    }

    // after we've processed all of the meshes (if any) we then recursively process each of the children nodes
    for (unsigned int i = 0; i < node->mNumChildren; i++) {
        process_node(node->mChildren[i], scene);
    }
}

std::unique_ptr<Mesh> Model::process_mesh(aiMesh *mesh, const aiScene *scene)
{
    // data to fill
    std::vector<Vertex> vertices;
    std::vector<unsigned int> indices;
    std::vector<Texture> textures;

    // walk through each of the mesh's vertices
    for (unsigned int i = 0; i < mesh->mNumVertices; i++) {
        Vertex vertex;
        glm::vec3 vector; // we declare a placeholder vector since assimp uses its own vector class that doesn't directly convert to glm's vec3 class so we transfer the data to this placeholder glm::vec3 first.
        // positions
        vector.x = mesh->mVertices[i].x;
        vector.y = mesh->mVertices[i].y;
        vector.z = mesh->mVertices[i].z;
        vertex.position = vector;
        // normals
        if (mesh->HasNormals()) {
            vector.x = mesh->mNormals[i].x;
            vector.y = mesh->mNormals[i].y;
            vector.z = mesh->mNormals[i].z;
            vertex.normal = vector;
        }
        // texture coordinates
        if (mesh->mTextureCoords[0]) { // does the mesh contain texture coordinates?
            glm::vec2 vec;
            // a vertex can contain up to 8 different texture coordinates. We thus make the assumption that we won't
            // use models where a vertex can have multiple texture coordinates so we always take the first set (0).
            vec.x = mesh->mTextureCoords[0][i].x;
            vec.y = mesh->mTextureCoords[0][i].y;
            vertex.tex_coord = vec;
            // tangent
            vector.x = mesh->mTangents[i].x;
            vector.y = mesh->mTangents[i].y;
            vector.z = mesh->mTangents[i].z;
            vertex.tangent = vector;
            // bitangent
            vector.x = mesh->mBitangents[i].x;
            vector.y = mesh->mBitangents[i].y;
            vector.z = mesh->mBitangents[i].z;
            vertex.bitangent = vector;
        } else {
            vertex.tex_coord = glm::vec2(0.0f, 0.0f);
        }

        vertices.push_back(vertex);
    }

    // now wak through each of the mesh's faces (a face is a mesh its triangle) and retrieve the corresponding vertex indices.
    for (unsigned int i = 0; i < mesh->mNumFaces; i++) {
        aiFace face = mesh->mFaces[i];

        // retrieve all indices of the face and store them in the indices vector
        for (unsigned int j = 0; j < face.mNumIndices; j++) {
            indices.push_back(face.mIndices[j]);
        }
    }

    // Write material index for current mesh.
    auto index = mesh->mMaterialIndex;

    // return a mesh object created from the extracted mesh data
    return std::make_unique<Mesh>(vertices, indices, index);
}

float get_float_from_color(const C_STRUCT aiColor4D &c)
{
    return (c.r, c.g, c.b) / 3.0f;
}

glm::vec3 get_vec3_from_color(const C_STRUCT aiColor4D &c)
{
    return glm::vec3{c.r, c.g, c.b};
}

std::unique_ptr<Material> Model::process_material(aiMaterial *material, const aiScene *scene)
{
    auto result = std::make_unique<Material>();

    result->mat_name = material->GetName().C_Str();

    C_STRUCT aiColor4D temp_color;
    C_STRUCT ai_real temp_float;

    // Common property defined in MTL file.
    if(AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_AMBIENT, &temp_color)) {
        result->ambient = get_vec3_from_color(temp_color);
    }

    if(AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_DIFFUSE, &temp_color)) {
        result->diffuse = get_vec3_from_color(temp_color);
    }

    if(AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_SPECULAR, &temp_color)) {
        result->specular = get_vec3_from_color(temp_color);
    }

    if(AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_EMISSIVE, &temp_color)) {
        result->emissive = get_vec3_from_color(temp_color);
    }

    if(AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_TRANSPARENT, &temp_color)) {
        result->transparency = get_vec3_from_color(temp_color);
    }

    if(AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_SHININESS, &temp_color)) {
        result->shiniess = get_float_from_color(temp_color);
    }

    if(AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_REFRACTI, &temp_color)) {
        result->ior = get_float_from_color(temp_color);
    }

    // Additional property for pbr model.
    if(AI_SUCCESS == aiGetMaterialColor(material, AI_MATKEY_COLOR_EMISSIVE, &temp_color)) {
        result->emissive = get_vec3_from_color(temp_color);
    }

    if(AI_SUCCESS == aiGetMaterialFloat(material, AI_MATKEY_METALLIC_FACTOR, &temp_float)) {
        result->metallic = temp_float;
    }

    if(AI_SUCCESS == aiGetMaterialFloat(material, AI_MATKEY_ROUGHNESS_FACTOR, &temp_float)) {
        result->roughness = temp_float;
    }

    result->normal_map = gen_material_texture(material, aiTextureType_NORMALS, "texture_normal");

    result->diffuse_map = gen_material_texture(material, aiTextureType_DIFFUSE, "texture_diffuse");
    result->specular_map = gen_material_texture(material, aiTextureType_SPECULAR, "texture_specular");

    result->emissive_map = gen_material_texture(material, aiTextureType_EMISSIVE, "texture_emissive");
    result->roughness_map = gen_material_texture(material, aiTextureType_DIFFUSE_ROUGHNESS, "texture_roughness");
    result->metallic_map = gen_material_texture(material, aiTextureType_METALNESS, "texture_metallic");
    // There's no metal roughness texture type in assimp, using aiTextureType_UNKNOWN instead.
    result->metal_roughness_map = gen_material_texture(material, aiTextureType_UNKNOWN, "texture_metal_roughness");
    result->ao_map = gen_material_texture(material, aiTextureType_LIGHTMAP, "texture_ao");

    return result;
}

std::unique_ptr<Texture> Model::gen_material_texture(aiMaterial *mat, aiTextureType type, const std::string &type_name)
{
    aiString str;
    if (AI_SUCCESS == mat->GetTexture(type, 0, &str)) {
        std::string filename = std::string(str.C_Str());
        filename = directory + '/' + filename;

        int width, height, n_components;
        unsigned char *data = stbi_load(filename.c_str(), &width, &height, &n_components, 0);
        auto texture = load_texture_2d(width, height, n_components, data);
        stbi_image_free(data);

        return texture;
    }

    return nullptr;
}
