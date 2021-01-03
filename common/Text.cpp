#include "Text.h"

#include <algorithm>

std::map<GLchar, TextRenderer::Character> TextRenderer::Characters;
GLuint TextRenderer::VAO, TextRenderer::VBO;
GLuint TextRenderer::textRendererVS, TextRenderer::textRendererFS;
std::shared_ptr<Shader> TextRenderer::textShader;
FT_Library TextRenderer::ft;
FT_Face TextRenderer::face;

static bool TextRendererRenderState = false;
static bool TextRendererInit = false;

extern int g_width, g_height;

void TextRenderer::Init(const char *path, size_t size)
{
    /** VAO, VBO **/
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);

    // Bind buffers
    glBindVertexArray(VAO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    glBufferData(GL_ARRAY_BUFFER, sizeof(GLfloat) * 6 * 4, nullptr, GL_DYNAMIC_DRAW);
    // Specify the vertex attributes
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(GLfloat), 0);
    // Unbind
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    textShader = Shader::LoadShader("shaders/text.vs", "shaders/text.fs");

    LoadFont(path, size);
    GenerateTextTextures();
    UnloadFont();

    TextRendererInit = true;
}

void TextRenderer::Text(glm::vec2 pos, float scale, const glm::vec3 &color, const std::string &text)
{
    assert(TextRendererRenderState && TextRendererInit);

    textShader->use();
    textShader->setVec3("textColor", color);
    textShader->setInt("text", 0);
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(VAO);
    // Find the max bearing
    float maxBearingY = std::numeric_limits<float>::min();
    std::for_each(text.begin(), text.end(), [&maxBearingY](char p){
        auto ch = Characters[p];
        maxBearingY = std::max((float)ch.Bearing.y, maxBearingY);
    });
    pos.y = g_height - pos.y - maxBearingY;

    // Iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = Characters[*c];

        GLfloat xpos = pos.x + ch.Bearing.x * scale;
        GLfloat ypos = pos.y - (ch.Size.y - ch.Bearing.y) * scale;

        GLfloat w = ch.Size.x * scale;
        GLfloat h = ch.Size.y * scale;

        // Update VBO for each character
        GLfloat vertices[6][4] = {
            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos,     ypos,       0.0, 1.0 },
            { xpos + w, ypos,       1.0, 1.0 },

            { xpos,     ypos + h,   0.0, 0.0 },
            { xpos + w, ypos,       1.0, 1.0 },
            { xpos + w, ypos + h,   1.0, 0.0 }
        };
        // Render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // Update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, VBO);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // Be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // Render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // Now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        pos.x += (ch.Advance >> 6) * scale; // Bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::LoadFont(const char *path, size_t size)
{
    if (FT_Init_FreeType(&ft))
        std::cerr << "[FREETYPE] Could not init FreeType Library" << '\n';
    if (FT_New_Face(ft, "fonts/arial.ttf", 0, &face))
        std::cerr << "[FREETYPE] Failed to load font" << '\n';
    // Font size
    FT_Set_Pixel_Sizes(face, 0, size);
}

void TextRenderer::UnloadFont()
{
    // Release the font
    FT_Done_Face(face);
    FT_Done_FreeType(ft);
}

void TextRenderer::GenerateTextTextures()
{
    // Disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // Load first 128 characters of ASCII set
    for (GLubyte c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // Generate texture
        GLuint texture;
        glGenTextures(1, &texture);
        glBindTexture(GL_TEXTURE_2D, texture);
        glTexImage2D(
            GL_TEXTURE_2D,
            0,
            GL_RED,
            face->glyph->bitmap.width,
            face->glyph->bitmap.rows,
            0,
            GL_RED,
            GL_UNSIGNED_BYTE,
            face->glyph->bitmap.buffer
        );
        // Set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // Now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            (GLuint)face->glyph->advance.x
        };
        Characters.insert(std::pair<GLchar, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
}

void TextRenderer::Begin(glm::mat4 &proj)
{
    textShader->use();
    textShader->setMat4("projection", proj);

    TextRendererRenderState = true;
}

void TextRenderer::End()
{
    TextRendererRenderState = false;
}

