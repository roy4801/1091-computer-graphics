/**
 * @file Text.h
 * @brief A small Text Renderer from learnopengl.com
 */
#pragma once
#ifndef TEXT_H
#define TEXT_H

#include <string>
#include <map>

#include <glad/glad.h>
#include <glm/glm.hpp>

#include "Shader.h"

#include <ft2build.h>
#include FT_FREETYPE_H

/**
 * Example of a Text Renderer
 * Reference : https://learnopengl-cn.github.io/06%20In%20Practice/02%20Text%20Rendering/
 * [roy4801](https://github.com/roy4801)
 */

/**
 * @class TextRenderer
 */
class TextRenderer
{
public:
    struct Character {
        GLuint     TextureID;  // ID handle of the glyph texture
        glm::ivec2 Size;       // Size of glyph
        glm::ivec2 Bearing;    // Offset from baseline to left/top of glyph
        GLuint     Advance;    // Horizontal offset to advance to next glyph
    };

    static void Init(const char *path, size_t size);
    static void Begin(glm::mat4 &proj);
    static void End();
    static void Text(glm::vec2 pos, float scale, const glm::vec3 &color, const std::string &text);

private:
    static void LoadFont(const char *path, size_t size);
    static void UnloadFont();
    static void GenerateTextTextures();

    static std::map<GLchar, Character> Characters;
    static GLuint VAO, VBO;
    static GLuint textRendererVS, textRendererFS;
    static std::shared_ptr<Shader> textShader;
    static FT_Library ft;
    static FT_Face face;
};

#endif //TEXT_H
