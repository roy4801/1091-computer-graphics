/**
 * @file Shader.h
 * @brief Shader Class from LearnOpenGL.com
 */
#pragma once
#ifndef SHADER_H
#define SHADER_H

#include <glad/glad.h>
#include <glm/glm.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <string>
#include <fstream>
#include <sstream>
#include <iostream>
#include <memory>

#include "Utils.h"

struct ShaderFile
{
    ShaderFile(const std::string path_, const char *source=nullptr)
        : path(path_)
    {
    }

    std::string path;
    const char *source = nullptr;
};

class Shader
{
public:
    unsigned int programID;

    Shader(ShaderFile vert, ShaderFile frag, ShaderFile geometry={"", nullptr});

    static std::shared_ptr<Shader> LoadShader(const char *vertPath, const char *fragPath, const char *geoPath="");
    static std::shared_ptr<Shader> LoadShaderFromSource(const char *vertSrc, const char *fragSrc, const char *geoSrc=nullptr);

    void use()
    {
        glUseProgram(programID);
    }

    void setBool(const std::string &name, bool value) const
    {
        glUniform1i(glGetUniformLocation(programID, name.c_str()), (int)value);
    }
    void setInt(const std::string &name, int value) const
    {
        glUniform1i(glGetUniformLocation(programID, name.c_str()), value);
    }
    void setFloat(const std::string &name, float value) const
    {
        glUniform1f(glGetUniformLocation(programID, name.c_str()), value);
    }
    void setVec2(const std::string &name, const glm::vec2 &value) const
    {
        glUniform2fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
    }
    void setVec2(const std::string &name, float x, float y) const
    {
        glUniform2f(glGetUniformLocation(programID, name.c_str()), x, y);
    }
    void setVec3(const std::string &name, const glm::vec3 &value) const
    {
        glUniform3fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
    }
    void setVec3(const std::string &name, float x, float y, float z) const
    {
        glUniform3f(glGetUniformLocation(programID, name.c_str()), x, y, z);
    }
    void setVec4(const std::string &name, const glm::vec4 &value) const
    {
        glUniform4fv(glGetUniformLocation(programID, name.c_str()), 1, &value[0]);
    }
    void setVec4(const std::string &name, float x, float y, float z, float w)
    {
        glUniform4f(glGetUniformLocation(programID, name.c_str()), x, y, z, w);
    }
    void setMat2(const std::string &name, const glm::mat2 &mat) const
    {
        glUniformMatrix2fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }
    void setMat3(const std::string &name, const glm::mat3 &mat) const
    {
        glUniformMatrix3fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }
    void setMat4(const std::string &name, const glm::mat4 &mat) const
    {
        glUniformMatrix4fv(glGetUniformLocation(programID, name.c_str()), 1, GL_FALSE, glm::value_ptr(mat));
    }

    GLint getUniformLocation(const char *name) const
    {
        return glGetUniformLocation(programID, name);
    }
    GLint getAttributeLocation(const char *name) const
    {
        return glGetAttribLocation(programID, name);
    }

private:

    void checkCompileErrors(GLuint shader, std::string type);
    uint32_t compileShader(GLenum type, const char **src);
};

#endif