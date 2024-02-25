// ---------------------------------------------------
// Comes from:
// https://learnopengl.com/In-Practice/Text-Rendering
// ---------------------------------------------------

#ifndef __TEXT_FT
#define __TEXT_FT

#include <ft2build.h>
#include FT_FREETYPE_H
#include <fstream>

#include <map>
#include <iostream>

// Holds all state information relevant to a character as loaded using FreeType
struct Character {
    unsigned int TextureID; // ID handle of the glyph texture
    glm::ivec2   Size;      // Size of glyph
    glm::ivec2   Bearing;   // Offset from baseline to left/top of glyph
    unsigned int Advance;   // Horizontal offset to advance to next glyph
};

GLuint text_program;
std::map<GLchar, Character> text_Characters;
GLuint text_vao, text_vbo;

unsigned long getFTFileLength(std::ifstream& file)
{
    if (!file.good()) return 0;

    unsigned long pos = file.tellg();
    file.seekg(0, std::ios::end);
    unsigned long len = file.tellg();
    file.seekg(std::ios::beg);

    return len;
}

// --------------------------------------------------------------
GLchar* FTLoadShaderFile(const char* filename)
{
    std::ifstream file;
    file.open(filename, std::ios::in); // opens as ASCII!
    if (!file) { printf("Can't open file %s !\n", filename); exit(1); }

    unsigned long len = getFTFileLength(file);

    if (len == 0) { printf("File %s is empty!\n", filename); exit(1); };   // Error: Empty File

    GLchar* ShaderSource = new GLchar[len + 1];
    if (ShaderSource == NULL) { printf("Can't reserve memory %d \n", len + 1); exit(1); }   // can't reserve memory

    // len isn't always strlen cause some characters are stripped in ascii read...
    // it is important to 0-terminate the real length later, len is just max possible value...
    ShaderSource[len] = 0;

    unsigned int i = 0;
    while (file.good())
    {
        ShaderSource[i] = file.get();
        if (!file.eof())
            i++;
    }

    ShaderSource[i] = 0;
    file.close();
    return ShaderSource;
}

// --------------------------------------------------------------
void FTCheckForErrors_Shader(GLuint shader)
{
    GLint status;
    glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
    if (status != GL_TRUE)
    {
        printf("Blad!\n");
        GLint logLength;
        glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);
        char* log = new char[logLength];
        glGetShaderInfoLog(shader, logLength, NULL, log);
        printf("LOG: %s\n", log);
        delete[] log;
    }
}

// --------------------------------------------------------------
void FTCheckForErrors_Program(GLuint program, GLenum mode)
{
    GLint status;
    glGetProgramiv(program, mode, &status);
    if (status != GL_TRUE)
    {
        switch (mode)
        {
        case GL_LINK_STATUS:
            printf("Blad konsolidacji programu!\n");
            break;
        case GL_VALIDATE_STATUS:
            printf("Blad walidacji programu!\n");
            break;
        default:
            printf("Inny blad programu!\n");

        }
        GLint logLength;
        glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);
        char* log = new char[logLength];
        glGetProgramInfoLog(program, logLength, NULL, log);
        printf("LOG: %s\n", log);
        delete[] log;
    }
}
// --------------------------------------------------------------
GLuint FTLoadShader(GLuint MODE, const char* filename)
{
    // utworzenie obiektu shadera
    GLuint shader = glCreateShader(MODE);

    // Wczytanie kodu shadera z pliku
    GLchar* code = FTLoadShaderFile(filename);

    glShaderSource(shader, 1, &code, NULL);
    glCompileShader(shader);
    FTCheckForErrors_Shader(shader);

    delete[] code;
    return shader;
}

// --------------------------------------------------------------
void FTLinkAndValidateProgram(GLuint program)
{
    // Konsolidacja programu
    glLinkProgram(program);
    FTCheckForErrors_Program(program, GL_LINK_STATUS);

    // Walidacja programu
    glValidateProgram(program);
    FTCheckForErrors_Program(program, GL_VALIDATE_STATUS);
}

// ----------------------------------------------------------------------------------
void RenderText(std::string text, float x, float y, float scale = 1.0f, glm::vec3 color = glm::vec3(1.0, 1.0, 1.0))
{
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // activate corresponding render state
    glUseProgram(text_program);
    glUniform3f(glGetUniformLocation(text_program, "textColor"), color.x, color.y, color.z);
    int Window_Width = glutGet(GLUT_WINDOW_WIDTH);
    int Window_Height = glutGet(GLUT_WINDOW_HEIGHT);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(Window_Width), 0.0f, static_cast<float>(Window_Height));

    glUniformMatrix4fv(glGetUniformLocation(text_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));
    glActiveTexture(GL_TEXTURE0);
    glBindVertexArray(text_vao);

    // iterate through all characters
    std::string::const_iterator c;
    for (c = text.begin(); c != text.end(); c++)
    {
        Character ch = text_Characters[*c];

        float xpos = x + ch.Bearing.x * scale;
        float ypos = y - (ch.Size.y - ch.Bearing.y) * scale;

        float w = ch.Size.x * scale;
        float h = ch.Size.y * scale;
        // update VBO for each character
        float vertices[6][4] = {
            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos,     ypos,       0.0f, 1.0f },
            { xpos + w, ypos,       1.0f, 1.0f },

            { xpos,     ypos + h,   0.0f, 0.0f },
            { xpos + w, ypos,       1.0f, 1.0f },
            { xpos + w, ypos + h,   1.0f, 0.0f }
        };
        // render glyph texture over quad
        glBindTexture(GL_TEXTURE_2D, ch.TextureID);
        // update content of VBO memory
        glBindBuffer(GL_ARRAY_BUFFER, text_vbo);
        glBufferSubData(GL_ARRAY_BUFFER, 0, sizeof(vertices), vertices); // be sure to use glBufferSubData and not glBufferData

        glBindBuffer(GL_ARRAY_BUFFER, 0);
        // render quad
        glDrawArrays(GL_TRIANGLES, 0, 6);
        // now advance cursors for next glyph (note that advance is number of 1/64 pixels)
        x += (ch.Advance >> 6) * scale; // bitshift by 6 to get value in pixels (2^6 = 64 (divide amount of 1/64th pixels by 64 to get amount of pixels))
    }
    glBindVertexArray(0);
    glBindTexture(GL_TEXTURE_2D, 0);

    glDisable(GL_BLEND);
}

// ----------------------------------------------------------------------------------
void InitText(const char *font_filename, int font_size)
{

    // Fonts
    text_program = glCreateProgram();
    glAttachShader( text_program, FTLoadShader(GL_VERTEX_SHADER, "shaders/text-ft-vertex.glsl"));
    glAttachShader( text_program, FTLoadShader(GL_FRAGMENT_SHADER, "shaders/text-ft-fragment.glsl"));
    FTLinkAndValidateProgram( text_program );

    int Window_Width = glutGet(GLUT_WINDOW_WIDTH);
    int Window_Height = glutGet(GLUT_WINDOW_HEIGHT);
    glm::mat4 projection = glm::ortho(0.0f, static_cast<float>(Window_Width), 0.0f, static_cast<float>(Window_Height));
    glUseProgram(text_program);

    glUniformMatrix4fv(glGetUniformLocation(text_program, "projection"), 1, GL_FALSE, glm::value_ptr(projection));

    // FreeType
    // --------
    FT_Library ft;
    // All functions return a value different than 0 whenever an error occurred
    if (FT_Init_FreeType(&ft))
        std::cout << "ERROR::FREETYPE: Could not init FreeType Library" << std::endl;

    // load font as face
    FT_Face face;
    if (FT_New_Face(ft, font_filename, 0, &face))
        std::cout << "ERROR::FREETYPE: Failed to load font" << std::endl;

    // set size to load glyphs as
    FT_Set_Pixel_Sizes(face, 0, font_size);

    // disable byte-alignment restriction
    glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

    // load first 128 characters of ASCII set
    for (unsigned char c = 0; c < 128; c++)
    {
        // Load character glyph
        if (FT_Load_Char(face, c, FT_LOAD_RENDER))
        {
            std::cout << "ERROR::FREETYTPE: Failed to load Glyph" << std::endl;
            continue;
        }
        // generate texture
        unsigned int texture;
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
        // set texture options
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        // now store character for later use
        Character character = {
            texture,
            glm::ivec2(face->glyph->bitmap.width, face->glyph->bitmap.rows),
            glm::ivec2(face->glyph->bitmap_left, face->glyph->bitmap_top),
            face->glyph->advance.x
        };
        text_Characters.insert(std::pair<char, Character>(c, character));
    }
    glBindTexture(GL_TEXTURE_2D, 0);
    // destroy FreeType once we're finished
    FT_Done_Face(face);
    FT_Done_FreeType(ft);


	// -----------------------------------
    // configure VAO/VBO for texture quads
    // -----------------------------------
    glGenVertexArrays(1, &text_vao);
    glBindVertexArray(text_vao);
    glGenBuffers(1, &text_vbo);
    glBindBuffer(GL_ARRAY_BUFFER, text_vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(float) * 6 * 4, NULL, GL_DYNAMIC_DRAW);
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(0, 4, GL_FLOAT, GL_FALSE, 4 * sizeof(float), 0);

    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

}

#endif // __TEXT_FT
