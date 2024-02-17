#define _USE_MATH_DEFINES
#include <stdio.h>
#include <GL/glew.h>
#include <GL/freeglut.h>
#include <vector>
#include <cmath>
#include <glm/glm.hpp>
#include <cstdlib>
#include <ctime>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

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
	-1.0f, -1.0f,  0.0f,
	 1.0f, -1.0f,  0.0f,
	-1.0f,  1.0f,  0.0f,
	 1.0f, -1.0f,  0.0f,
	-1.0f,  1.0f,  0.0f,
	 1.0f,  1.0f,  0.0f,
	 0.0f, -1.0f,  1.0f,
	 0.0f, -1.0f, -1.0f,
	 0.0f,  1.0f,  1.0f,
	 0.0f, -1.0f, -1.0f,
	 0.0f,  1.0f,  1.0f,
	 0.0f,  1.0f, -1.0f,
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
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
	0.0f, 0.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 0.0f,
	0.0f, 1.0f,
	1.0f, 1.0f,
};



constexpr float one_degree = M_PI / 180.f;
std::vector<glm::mat4> positions;

// ---------------------------------------
void DisplayScene()
{
	// Czyszczenie bufora koloru i glebokosci
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Obliczanie macierzy widoku
	//matView = UpdateViewMatrix();


	// Wlaczenie programu
	glUseProgram(idProgram);

	for (auto& pos : positions) {
		// Przekazanie macierzy
		glUniformMatrix4fv(glGetUniformLocation(idProgram, "matProj"), 1, GL_FALSE, glm::value_ptr(matProj));
		glUniformMatrix4fv(glGetUniformLocation(idProgram, "matView"), 1, GL_FALSE, glm::value_ptr(matView));
		glUniformMatrix4fv(glGetUniformLocation(idProgram, "matModel"), 1, GL_FALSE, glm::value_ptr(pos));

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
		glDrawArrays(GL_TRIANGLES, 0, sizeof(vertices_coord) / sizeof(GLfloat));
		glBindVertexArray(0);
	}

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


	matView = glm::mat4(1.0);
	matView = glm::translate(matView, glm::vec3(0, 0, -10));
	matView = glm::rotate(matView, one_degree * 45, glm::vec3(1, 0, 0));

	std::srand(static_cast<unsigned int>(std::time(nullptr)));
	for (int i = 0; i < 10; ++i) {
		glm::mat4 mat = glm::mat4(1.0);
		mat = glm::translate(mat, glm::vec3(
			(std::rand() % 21) - 10, (std::rand() % 21) - 10, (std::rand() % 21) - 10
		));
		positions.push_back(mat);
	}

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



	// ------------------------------------
	// NOWE: Wczytanie pliku tekstury
	// ------------------------------------

	int tex_width, tex_height, tex_n;
	unsigned char* tex_data;

	// tylko raz w calym programie
	stbi_set_flip_vertically_on_load(true);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// wczytywanie pliku graficznego
	tex_data = stbi_load("flower32bit.png", &tex_width, &tex_height, &tex_n, 0);
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
	case 27:	// ESC key
		// opuszczamy glowna petle
		glutLeaveMainLoop();
		break;
	case 'w':
		matView = glm::rotate(matView, -one_degree * 2, glm::vec3(1, 0, 0));
		break;
	case 's':
		matView = glm::rotate(matView, one_degree * 2, glm::vec3(1, 0, 0));
		break;
	case 'd':
		matView = glm::rotate(matView, one_degree * 2, glm::vec3(0, 1, 0));
		break;
	case 'a':
		matView = glm::rotate(matView, -one_degree * 2, glm::vec3(0, 1, 0));
		break;
	case 'q':
		matView = glm::rotate(matView, one_degree * 2, glm::vec3(0, 0, 1));
		break;
	case 'e':
		matView = glm::rotate(matView, -one_degree * 2, glm::vec3(0, 0, 1));
		break;
	case ' ':	// Space key
		printf("SPACE!\n");
		// ponowne renderowanie
		glutPostRedisplay();
		break;
	}

	glutPostRedisplay();
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
