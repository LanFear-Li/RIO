#pragma once

#include "scene/mesh.hpp"
#include "scene/material.hpp"

#include <assimp/Importer.hpp>
#include <assimp/scene.h>
#include <assimp/postprocess.h>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <map>
#include <vector>

class Model
{
public:
    // Model data storage.
    vector<Mesh> meshes;
    vector<Material> materials;
    std::string directory;
    bool gammaCorrection;

    // constructor, expects a filepath to a 3D model.
    explicit Model(std::string const &path, bool gamma = false);

private:
    // loads a model with supported ASSIMP extensions from file and stores the resulting meshes in the meshes vector.
    void loadModel(std::string const &path);

    // processes a node in a recursive fashion. Processes each individual mesh located at the node and repeats this process on its children nodes (if any).
    void processNode(aiNode *node, const aiScene *scene);
    Mesh processMesh(aiMesh *mesh, const aiScene *scene);
    Material processMaterial(aiMaterial *material, const aiScene *scene);

    // Helper function for material loading.
    std::unique_ptr<Texture> genMaterialTexture(aiMaterial *mat, aiTextureType type, std::string typeName);
};
