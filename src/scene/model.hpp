#pragma once

#include "scene/mesh.hpp"
#include "scene/material.hpp"
#include "render/shader.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

using namespace std;

unsigned int TextureFromFile(const char *path, const string &directory, bool gamma = false);

class Model
{
public:
    // Model data storage.
    vector<Mesh> meshes;
    vector<Material> materials;
    string directory;
    bool gammaCorrection;

    // constructor, expects a filepath to a 3D model.
    explicit Model(string const &path, bool gamma = false);

    static unsigned int load_texture(const char* path);

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(string const &path);

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    Material processMaterial(aiMaterial *material, const aiScene *scene);

    // Helper function for material loading.
    std::unique_ptr<Texture> genMaterialTexture(aiMaterial *mat, aiTextureType type, string typeName);
    glm::vec3 genMaterialColor(aiMaterial *mat, aiTextureType type, string typeName);
    float genMaterialFloat(aiMaterial *mat, aiTextureType type, string typeName);
};
