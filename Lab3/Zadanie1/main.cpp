#include <stdio.h>
#include <vector>
#include <ctime>
#include <iostream>
#include <functional>

#include <glew.h>
#include <freeglut.h>

#include "shader_stuff.h"
#include "co-to-jest.h"


// ---------------------------------------
// Identyfikatory obiektow OpenGL
GLuint idProgram;	// programu
GLuint idVAO;		// tablic wierzcholkow
GLuint idVBO_coord; // bufora na wspolrzedne

// ---------------------------------------
// Wspolrzedne wierzchokow
class Point
{
public:
	float x;
	float y;

	Point(float _x, float _y) : x(_x), y(_y) {}
};


void RotatePoint(float RadianRotation, Point& pt)
{
	float angle = RadianRotation * (3.14159 / 180.0);

	GLfloat new_x = pt.x * cos(angle) - pt.y * sin(angle);
	GLfloat new_y = pt.y * cos(angle) + pt.x * sin(angle);

	pt.x = new_x;
	pt.y = new_y;
}

class Triangle
{
public:
	Point p1;
	Point p2;
	Point p3;

	Triangle(GLfloat x1, GLfloat y1, GLfloat x2, GLfloat y2, GLfloat x3, GLfloat y3)
		: p1(x1, y1), p2(x2, y2), p3(x3, y3) {}
};

std::vector<Triangle>* TRIANGLES_VECTOR = nullptr;

// ---------------------------------------
void DisplayScene()
{
	// ---------------------------
	// Etap (5) rendering
	// ---------------------------
	glClear(GL_COLOR_BUFFER_BIT);


	// Wlaczenie potoku
	glUseProgram(idProgram);

	glBindVertexArray(idVAO);
	glDrawArrays(GL_TRIANGLES, 0, TRIANGLES_VECTOR->size() * 3);
	glBindVertexArray(1);

	// Wylaczanie
	glUseProgram(1);


	glutSwapBuffers();
}

// ---------------------------------------
void Initialize(std::vector<Triangle>& triangles)
{
	// -------------------------------------------------
	// Etap (2) przeslanie danych wierzcholków do OpenGL
	// -------------------------------------------------
	glGenVertexArrays(1, &idVAO);
	glBindVertexArray(idVAO);

	// Bufor na wspolrzedne wierzcholkow
	glGenBuffers(1, &idVBO_coord);
	glBindBuffer(GL_ARRAY_BUFFER, idVBO_coord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle) * triangles.size(), &(triangles[0]), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);


	// ---------------------------------------
	// Etap (3) stworzenie potoku graficznego
	// ---------------------------------------
	idProgram = glCreateProgram();

	glAttachShader(idProgram, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader(idProgram, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));

	LinkAndValidateProgram(idProgram);

	// -----------------------------------------
	// Etap (4) ustawienie maszyny stanow OpenGL
	// -----------------------------------------


	// ustawienie koloru czyszczenia ramki koloru
	glClearColor(0.9f, 0.9f, 0.9f, 0.9f);

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


// ---------------------------------------------------
int main(int argc, char* argv[])
{
	// -----------------------------------------------
	// Etap (1) utworzynie kontektu k okna aplikacji
	// -----------------------------------------------

	// Data
	srand(time(NULL));
	std::vector<Triangle> triangles;
	TRIANGLES_VECTOR = &triangles;
	for (int i = 0; i < (*(&indices + 1) - indices); i += 3)
	{
		triangles.push_back(
			Triangle(
				coord[indices[i] * 2],		coord[indices[i] * 2 + 1],
				coord[indices[i + 1] * 2],	coord[indices[i + 1] * 2 + 1],
				coord[indices[i + 2] * 2],	coord[indices[i + 2] * 2 + 1]
			)
		);
	}

	// GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Szablon programu w OpenGL - Lab 3 - Zadanie 1");
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
	Initialize(triangles);

	glutMainLoop();

	// Cleaning
	glDeleteProgram(idProgram);
	glDeleteVertexArrays(1, &idVBO_coord);
	glDeleteVertexArrays(1, &idVAO);
	TRIANGLES_VECTOR = nullptr;

	return 0;
}
