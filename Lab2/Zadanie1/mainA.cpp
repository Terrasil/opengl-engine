#include <stdio.h>
#include <vector>
#include <ctime>
#include <functional>
#include <iostream>

#include <glew.h>
#include <freeglut.h>

#include "shader_stuff.h"


// ---------------------------------------
// Identyfikatory obiektow OpenGL

GLuint idProgram;	// programu
GLuint idVAO;		// tablic wierzcholkow
GLuint idVBO_coord; // bufora na wspolrzedne 
GLuint idVBO_colors; // bufora na colory

// ---------------------------------------
// Wspolrzedne wierzchokow

// Random floaf -1.0 to 1.0
float randomFloat()
{
	return (((float)(rand()) / (float)(RAND_MAX))*2)-1.f;
}
float randf()
{
	return static_cast<float>(std::rand()) / RAND_MAX;
}

class Point
{
public:
	float x;
	float y;

	Point(float _x, float _y) : x(_x), y(_y) {}
};

struct Color {
	float r, g, b;
	Color(float _r, float _g, float _b)
		: r(_r), g(_g), b(_b) {}
};

class Triangle
{
public:
	Point p1;
	Point p2;
	Point p3;

	Triangle(float x1, float y1, float x2, float y2, float x3, float y3)
		: p1(x1, y1), p2(x2, y2), p3(x3, y3) {}
};

std::vector<Triangle> *TRIANGLES_VECTOR = nullptr;


// ---------------------------------------
void DisplayScene()
{
	// ---------------------------
	// Etap (5) rendering
	// ---------------------------
	glClear(GL_COLOR_BUFFER_BIT);


	// Wlaczenie potoku
	glUseProgram(idProgram);


	// Generowanie obiektow na ekranie
	glBindVertexArray(idVAO);
	glDrawArrays(GL_TRIANGLES, 0, TRIANGLES_VECTOR->size() * 3);
	glBindVertexArray(0);


	// Wylaczanie
	glUseProgram(0);


	glutSwapBuffers();
}

// ---------------------------------------
void Initialize(std::vector<Triangle> &triangles, std::vector<Color> &colors)
{
	// -------------------------------------------------
	// Etap (2) przeslanie danych wierzcholków do OpenGL
	// -------------------------------------------------
	glGenVertexArrays(1, &idVAO);
	glBindVertexArray(idVAO);

	// Bufor na wspolrzedne wierzcholkow
	glGenBuffers(1, &idVBO_coord);
	glBindBuffer(GL_ARRAY_BUFFER, idVBO_coord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Point) * 3 * triangles.size(), &(triangles[0]), GL_STATIC_DRAW);
	glEnableVertexAttribArray(0);
	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);

	glGenBuffers(1, &idVBO_colors);
	glBindBuffer(GL_ARRAY_BUFFER, idVBO_colors);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Color) * colors.size(), &(colors[0]), GL_STATIC_DRAW);
	glEnableVertexAttribArray(1);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);

	glBindVertexArray(0);
	glBindVertexArray(1);

	// ---------------------------------------
	// Etap (3) stworzenie potoku graficznego
	// ---------------------------------------
	idProgram = glCreateProgram();

	glAttachShader(idProgram, LoadShader(GL_VERTEX_SHADER, "vertexA.glsl"));
	glAttachShader(idProgram, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));

	LinkAndValidateProgram(idProgram);



	// -----------------------------------------
	// Etap (4) ustawienie maszyny stanow OpenGL
	// -----------------------------------------

	// ustawienie koloru czyszczenia ramki koloru
	glClearColor(0.9f, 0.9f, 0.9f, 0.9f);
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

}

// ---------------------------------------
// Funkcja wywolywana podczas zmiany rozdzielczosci ekranu
void Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
}



// ---------------------------------------------------
// Funkcja wywolywana podczas wcisniecia klawisza ASCII
void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:	// ESC key
		// opuszczamy glowna petle
		glutLeaveMainLoop();
		break;

	case ' ':	// Space key
		printf("SPACE!\n");
		// ponowne renderowanie
		glutPostRedisplay();
		break;
	}
}

void initializeTriangles(std::vector<Triangle>& triangles, int N) {
	srand(time(NULL));
	TRIANGLES_VECTOR = &triangles;
	for (int i = 0; i < N; i++) {
		triangles.push_back(Triangle(randomFloat(), randomFloat(), randomFloat(), randomFloat(), randomFloat(), randomFloat()));
	}
}
void initializeColors(std::vector<Color>& colors, int N) {
	srand(time(NULL));
	for (int i = 0; i < N; i++){
		colors.push_back(Color(randf(), randf(), randf()));
	}
}

// ---------------------------------------------------
int main(int argc, char* argv[])
{
	// -----------------------------------------------
	// Etap (1) utworzynie kontektu i okna aplikacji
	// -----------------------------------------------

	// Data
	int N = 100;
	std::vector<Triangle> triangles;
	initializeTriangles(triangles, N);
	std::vector<Color> colors;
	initializeColors(colors, N*3);

	// GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Szablon programu w OpenGL");
	glutDisplayFunc(DisplayScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);


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

	// Inicjalizacja
	Initialize(triangles, colors);

	glutMainLoop();

	// Cleaning
	glDeleteProgram(idProgram);
	glDeleteVertexArrays(1, &idVBO_coord);
	glDeleteVertexArrays(1, &idVBO_colors);
	glDeleteVertexArrays(1, &idVAO);
	TRIANGLES_VECTOR = nullptr;
	return 0;
}
