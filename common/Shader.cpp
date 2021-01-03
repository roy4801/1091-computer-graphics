#include "Shader.h"

/**
 * Load Shader from the path
 * @param vertSrc src of vertex shader
 * @param fragSrc src of fragment shader
 * @param geoSrc src of geometry shader
 * @return Shader
 */
std::shared_ptr<Shader> Shader::LoadShader(const char *vertPath, const char *fragPath, const char *geoPath)
{
    return std::make_shared<Shader>(ShaderFile{std::string(vertPath), nullptr},
                                    ShaderFile{std::string(fragPath), nullptr},
                                    ShaderFile{std::string(geoPath), nullptr});
}

/**
 * Load Shader from string in cpp
 * @param vertSrc src of vertex shader
 * @param fragSrc src of fragment shader
 * @param geoSrc src of geometry shader
 * @return Shader
 */
std::shared_ptr<Shader> Shader::LoadShaderFromSource(const char *vertSrc, const char *fragSrc, const char *geoSrc)
{
    return std::make_shared<Shader>(ShaderFile{"", vertSrc},
                                    ShaderFile{"", fragSrc},
                                    ShaderFile{"", geoSrc});
}

/**
 * Constructor of Shader
 * @param vert Vertex Shader
 * @param frag Fragment Shader
 * @param geometry Geometry Shader
 */
Shader::Shader(ShaderFile vert, ShaderFile frag, ShaderFile geometry)
{
    std::string vertexCode, fragmentCode, geometryCode;
    uint32_t vertProgram{}, fragProgram{}, geoProgram{};

    try
    {
        if(vert.source == nullptr)
        {
            vertexCode = ReadFile(vert.path);
            vert.source = vertexCode.c_str();
        }
        vertProgram = compileShader(GL_VERTEX_SHADER, &vert.source);

        if(frag.source == nullptr)
        {
            fragmentCode = ReadFile(frag.path);
            frag.source = fragmentCode.c_str();
        }
        fragProgram = compileShader(GL_FRAGMENT_SHADER, &frag.source);

        if(geometry.source == nullptr && !geometry.path.empty())
        {
            geometryCode = ReadFile(geometry.path);
            geometry.source = geometryCode.c_str();
        }
        if(geometry.source != nullptr)
            geoProgram = compileShader(GL_GEOMETRY_SHADER, &geometry.source);
    }
    catch (std::exception &e)
    {
        std::cerr << "Error: Shader file not successfully read" << '\n';
        if(vertProgram)
            glDeleteShader(vertProgram);
        if(fragProgram)
            glDeleteShader(fragProgram);
        if(geoProgram)
            glDeleteShader(geoProgram);
        throw;
    }

    programID = glCreateProgram();
    glAttachShader(programID, vertProgram);
    glAttachShader(programID, fragProgram);
    if(geoProgram)
        glAttachShader(programID, geoProgram);

    glLinkProgram(programID);
    checkCompileErrors(programID, "PROGRAM");

    glDeleteShader(vertProgram);
    glDeleteShader(fragProgram);
    if(geoProgram)
        glDeleteShader(geoProgram);
}

/**
* Utility function for checking shader compilation/linking errors.
* @param shader Shader
* @param type Type
*/
void Shader::checkCompileErrors(GLuint shader, std::string type)
{
    assert(!type.empty());

    GLint success;
    GLchar infoLog[1024];
    if(type != "PROGRAM")
    {
        glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
        if(!success)
        {
            glGetShaderInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "Shader Compiling Error [" << type << "]: " << infoLog << '\n';
        }
    }
    else
    {
        glGetProgramiv(shader, GL_LINK_STATUS, &success);
        if(!success)
        {
            glGetProgramInfoLog(shader, 1024, NULL, infoLog);
            std::cerr << "Shader Linking Error [" << type << "]: " << infoLog << '\n';
        }
    }
}

/**
 * Helper function for compiling a Shader
 * @param type Shader Type
 * @param src Shader Source
 * @return Shader ID
 */
uint32_t Shader::compileShader(GLenum type, const char **src)
{
    GLuint shader = glCreateShader(type);
    glShaderSource(shader, 1, src, nullptr);
    glCompileShader(shader);
    checkCompileErrors(shader,
       type == GL_VERTEX_SHADER ?
       "VERTEX" : type == GL_FRAGMENT_SHADER ?
              "FRAGMENT" : type == GL_GEOMETRY_SHADER ?
                       "GEOMETRY" : "");
    return shader;
}