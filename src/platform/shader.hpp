#pragma once

#include <glad/glad.h>
#include <glm/glm.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>

class Shader final
{
public:
    unsigned int ID;

    Shader() = delete;
    Shader(const char *vertexPath, const char *fragmentPath, const char *geometryPath = nullptr);
    Shader(const char *compPath);

private:
    std::string glsl_version = "#version 450\n";
    std::string path_vertex;
    std::string path_fragment;
    std::string path_geometry;

    // utility function for checking shader compilation/linking errors.
    // ------------------------------------------------------------------------
    void checkCompileErrors(GLuint shader, std::string type);

public:
    // activate the shader
    // ------------------------------------------------------------------------
    void use();

    // utility uniform functions
    // ------------------------------------------------------------------------
    void setBool(const std::string &name, bool value) const;

    // ------------------------------------------------------------------------
    void setInt(const std::string &name, int value) const;

    // ------------------------------------------------------------------------
    void setFloat(const std::string &name, float value) const;

    // ------------------------------------------------------------------------
    void setVec2(const std::string &name, const glm::vec2 &value) const;

    void setVec2(const std::string &name, float x, float y) const;

    // ------------------------------------------------------------------------
    void setVec3(const std::string &name, const glm::vec3 &value) const;

    void setVec3(const std::string &name, float x, float y, float z) const;

    // ------------------------------------------------------------------------
    void setVec4(const std::string &name, const glm::vec4 &value) const;

    void setVec4(const std::string &name, float x, float y, float z, float w);

    // ------------------------------------------------------------------------
    void setMat2(const std::string &name, const glm::mat2 &mat) const;

    // ------------------------------------------------------------------------
    void setMat3(const std::string &name, const glm::mat3 &mat) const;

    // ------------------------------------------------------------------------
    void setMat4(const std::string &name, const glm::mat4 &mat) const;
};
