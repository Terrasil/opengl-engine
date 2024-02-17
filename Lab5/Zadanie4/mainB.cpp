#include <stdio.h>
#include <glew.h>
#include <freeglut.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

// Lokalne pliki naglowkowe
#include "utilities.hpp"

// -----------------------------------------
// NOWE: plik do obslugi plikow graficznych
// -----------------------------------------
# define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


// Macierze transformacji i rzutowania
glm::mat4 matProj = glm::mat4(1.0);
glm::mat4 matView = glm::mat4(1.0);
glm::mat4 matModel = glm::mat4(1.0);


// Identyfikatory obiektow
GLuint idProgram;
GLuint idVAO;
GLuint idVBO_coord;
GLuint idVBO_color;
GLuint idVBO_uv;

GLuint idVAO2;
GLuint idVBO_coord2;
GLuint idVBO_color2;
GLuint idVBO_uv2;

// ---------------------------------------
// NOWE: identyfikator obiektu tekstury
// ---------------------------------------
GLuint idTexture;

// ---------------------------------------
// Dane trojkata
// ---------------------------------------
GLfloat vertices_coord[] =
{
	-1.0f, -1.0f, 0.0f,
	 1.0f, -1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,
};
GLfloat vertices_coord2[] =
{
	 1.0f, -1.0f, 0.0f,
	-1.0f,  1.0f, 0.0f,
	 1.0f,  1.0f, 0.0f,
};

GLfloat vertices_color[] =
{
	1.0f, 0.0f, 0.0f,
	0.0f, 1.0f, 0.0f,
	0.0f, 0.0f, 1.0f,
};


GLfloat vertices_uv[] =
{
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
};
GLfloat vertices_uv2[] =
{
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
};

int ticks = 0;


// ---------------------------------------
void DisplayScene()
{
	// Czyszczenie bufora koloru i glebokosci
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Obliczanie macierzy widoku
	matView = UpdateViewMatrix();


	// Wlaczenie programu
	glUseProgram(idProgram);


	// Przekazanie macierzy
	glUniformMatrix4fv(glGetUniformLocation(idProgram, "matProj"), 1, GL_FALSE, glm::value_ptr(matProj));
	glUniformMatrix4fv(glGetUniformLocation(idProgram, "matView"), 1, GL_FALSE, glm::value_ptr(matView));
	glUniformMatrix4fv(glGetUniformLocation(idProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(matModel));
	glUniform1i(glGetUniformLocation(idProgram, "spriteVFrames"), 7);
	glUniform1i(glGetUniformLocation(idProgram, "spriteHFrames"), 4);
	glUniform1i(glGetUniformLocation(idProgram, "vFrame"), (ticks % 28) % 7);
	glUniform1i(glGetUniformLocation(idProgram, "hFrame"), (ticks % 28) / 7);

	// -----------------------------------------------
	// NOWE: Aktywowanie jednostki teksturujacej nr 0,
	// przypisanie do niej obiektu tekstury oraz
	// polaczenie j.t. z uchwytem tekstury w shaderze
	// -----------------------------------------------
	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_2D, idTexture);
	glUniform1i(glGetUniformLocation(idProgram, "tex"), 0);


	// Rendering
	glBindVertexArray(idVAO);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);

	glBindVertexArray(idVAO2);
	glDrawArrays(GL_TRIANGLES, 0, 3);
	glBindVertexArray(0);


	// Wylaczanie
	glUseProgram(0);


	glutSwapBuffers();

}

// ---------------------------------------
void Initialize()
{

	// Ustawienia maszyny OpenGL
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.7f, 0.7f, 0.7f, 1.0f);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);


	// Potok
	idProgram = glCreateProgram();
	glAttachShader(idProgram, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader(idProgram, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
	LinkAndValidateProgram(idProgram);


	// VAO
	glGenVertexArrays(1, &idVAO);
	glBindVertexArray(idVAO);

	// Bufor na wspolrzedne wierzcholkow
	glGenBuffers(1, &idVBO_coord);
	glBindBuffer(GL_ARRAY_BUFFER, idVBO_coord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_coord), &vertices_coord[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	// Bufor na kolory
	glGenBuffers(1, &idVBO_color);
	glBindBuffer(GL_ARRAY_BUFFER, idVBO_color);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_color), &vertices_color[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	// -------------------------------------
	// NOWE: Bufor na wspolrzedne tekstury
	// -------------------------------------
	glGenBuffers(1, &idVBO_uv);
	glBindBuffer(GL_ARRAY_BUFFER, idVBO_uv);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_uv), &vertices_uv[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);


	// VAO2
	glGenVertexArrays(1, &idVAO2);
	glBindVertexArray(idVAO2);

	// Bufor na wspolrzedne wierzcholkow
	glGenBuffers(1, &idVBO_coord2);
	glBindBuffer(GL_ARRAY_BUFFER, idVBO_coord2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_coord2), &vertices_coord2[0], GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	// Bufor na kolory
	glGenBuffers(1, &idVBO_color2);
	glBindBuffer(GL_ARRAY_BUFFER, idVBO_color2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_color), &vertices_color[0], GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);

	// -------------------------------------
	// NOWE: Bufor na wspolrzedne tekstury
	// -------------------------------------
	glGenBuffers(1, &idVBO_uv2);
	glBindBuffer(GL_ARRAY_BUFFER, idVBO_uv2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices_uv2), &vertices_uv2[0], GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);

	glBindVertexArray(0);


	// ------------------------------------
	// NOWE: Wczytanie pliku tekstury
	// ------------------------------------

	int tex_width, tex_height, tex_n;
	unsigned char* tex_data;

	// tylko raz w calym programie
	stbi_set_flip_vertically_on_load(true);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// wczytywanie pliku graficznego
	tex_data = stbi_load("character.png", &tex_width, &tex_height, &tex_n, 0);
	if (tex_data == NULL) {
		printf("Image can�t be loaded!\n");
	}

	// ------------------------------------
	// NOWE: Utworzenie obiektu tekstury
	// ------------------------------------

	glGenTextures(1, &idTexture);
	glBindTexture(GL_TEXTURE_2D, idTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);


}

// ---------------------------------------
void Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	matProj = glm::perspective(glm::radians(80.0f), width / (float)height, 0.1f, 50.0f);
}

// --------------------------------------------------------------
void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27: // ESC key
		glutLeaveMainLoop();
		break;
	}

}

void FrameCallback(int i)
{
	glutPostRedisplay();
	ticks++;
	glutTimerFunc(1000.f / 10, FrameCallback, 0);
}

// ---------------------------------------------------
int main(int argc, char* argv[])
{
	// GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Programownie grafiki w OpenGL");


	glutDisplayFunc(DisplayScene);
	glutReshapeFunc(Reshape);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutMouseWheelFunc(MouseWheel);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeys);
	glutTimerFunc(1000.0 / 10.0, FrameCallback, 0);


	// GLEW
	glewExperimental = GL_TRUE;
	GLenum err = glewInit();
	if (GLEW_OK != err)
	{
		printf("GLEW Error\n");
		exit(1);
	}

	// OpenGL
	if (!GLEW_VERSION_3_2)
	{
		printf("Brak OpenGL 3.2!\n");
		exit(1);
	}


	Initialize();


	glutMainLoop();


	// Cleaning
	glDeleteProgram(idProgram);
	glDeleteVertexArrays(1, &idVBO_coord);
	glDeleteVertexArrays(1, &idVBO_color);
	glDeleteVertexArrays(1, &idVBO_uv);
	glDeleteVertexArrays(1, &idVAO);

	return 0;
}
