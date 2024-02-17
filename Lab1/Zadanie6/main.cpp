#include <stdio.h>
#include <vector>
#include <ctime>
#include <iostream>
#include <functional>

#include <glew.h>
#include <freeglut.h>

#include "shader_stuff.h"


// ---------------------------------------
// Identyfikatory obiektow OpenGL

GLuint idProgram1;	// programu
GLuint idVAO1;		// tablic wierzcholkow
GLuint idVBO_coord1; // bufora na wspolrzedne

GLuint idProgram_2;	// programu
GLuint idVAO_2;		// tablic wierzcholkow
GLuint idVBO_coord_2; // bufora na wspolrzedne

// ---------------------------------------
// Wspolrzedne wierzchokow

// Random floaf -1.0 to 1.0
float randomFloat()
{
	return (((float)(rand()) / (float)(RAND_MAX)) * 2) - 1.f;
}

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

	Triangle(float x1, float y1, float x2, float y2, float x3, float y3)
		: p1(x1, y1), p2(x2, y2), p3(x3, y3) {}
};


std::vector<Point> *POINT_VECTOR = nullptr;
std::vector<Triangle> *TRIANGLE_FILLING = nullptr;

void RotateTriangle(float RadianRotation, Triangle& tri)
{
	float angle = RadianRotation * (3.14159 / 180.0);

	float new_x1 = tri.p1.x * cos(angle) - tri.p1.y * sin(angle);
	float new_y1 = tri.p1.y * cos(angle) + tri.p1.x * sin(angle);

	float new_x2 = tri.p2.x * cos(angle) - tri.p2.y * sin(angle);
	float new_y2 = tri.p2.y * cos(angle) + tri.p2.x * sin(angle);

	float new_x3 = tri.p3.x * cos(angle) - tri.p3.y * sin(angle);
	float new_y3 = tri.p3.y * cos(angle) + tri.p3.x * sin(angle);

	tri.p1.x = new_x1;
	tri.p1.y = new_y1;
	tri.p2.x = new_x2;
	tri.p2.y = new_y2;
	tri.p3.x = new_x3;
	tri.p3.y = new_y3;
}


// ---------------------------------------
void DisplayScene()
{
	// ---------------------------
	// Etap (5) rendering
	// ---------------------------
	glClear(GL_COLOR_BUFFER_BIT);


	// Wlaczenie potoku
	glUseProgram(idProgram1);

	// Przekazanie rozmiaru okna
	GLint windowSizeLocation = glGetUniformLocation(idProgram1, "screenSize");
	glUniform2f(windowSizeLocation, (float) glutGet(GLUT_WINDOW_WIDTH), (float) glutGet(GLUT_WINDOW_HEIGHT));

	// Generowanie obiektow na ekranie
	glBindVertexArray(idVAO1);
	glDrawArrays(GL_LINE_LOOP, 0, POINT_VECTOR->size());
	glBindVertexArray(0);

	glUseProgram(0);

	glUseProgram(idProgram_2);

	glBindVertexArray(idVAO_2);
	glDrawArrays(GL_TRIANGLES, 0, TRIANGLE_FILLING->size()*3);
	glBindVertexArray(1);

	// Wylaczanie
	glUseProgram(1);


	glutSwapBuffers();
}

// ---------------------------------------
void Initialize(std::vector<Point> &points)
{
	// -------------------------------------------------
	// Etap (2) przeslanie danych wierzcholków do OpenGL
	// -------------------------------------------------
	glGenVertexArrays(1, &idVAO1);
	glBindVertexArray(idVAO1);

	// Bufor na wspolrzedne wierzcholkow
	glGenBuffers(1, &idVBO_coord1);
	glBindBuffer(GL_ARRAY_BUFFER, idVBO_coord1);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Point)*POINT_VECTOR->size(), &((*POINT_VECTOR)[0]), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);


	// ---------------------------------------
	// Etap (3) stworzenie potoku graficznego
	// ---------------------------------------
	idProgram1 = glCreateProgram();

	glAttachShader(idProgram1, LoadShader(GL_VERTEX_SHADER, "vertex1.glsl"));
	glAttachShader(idProgram1, LoadShader(GL_FRAGMENT_SHADER, "fragment1.glsl"));

	LinkAndValidateProgram(idProgram1);

	/////////////////////////////////////////////////////////////////////////////////////////
	// Tworzenie dodatkowego potoku graficznego

	glGenVertexArrays(1, &idVAO_2);
	glBindVertexArray(idVAO_2);

	// Bufor na wspolrzedne wierzcholkow
	glGenBuffers(1, &idVBO_coord_2);
	glBindBuffer(GL_ARRAY_BUFFER, idVBO_coord_2);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle)*POINT_VECTOR->size(), &((*TRIANGLE_FILLING)[0]), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	idProgram_2 = glCreateProgram();

	glAttachShader(idProgram_2, LoadShader(GL_VERTEX_SHADER, "vertex2.glsl"));
	glAttachShader(idProgram_2, LoadShader(GL_FRAGMENT_SHADER, "fragment2.glsl"));

	LinkAndValidateProgram(idProgram_2);

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
	std::vector<Point> points;
	std::vector<Triangle> filling;
	POINT_VECTOR = &points;
	TRIANGLE_FILLING = &filling;
	float degree = 0.f;
	for (int i = 0; i < 8; i++)
	{
		points.push_back(Point(0.0f, 0.75f));
		RotatePoint(degree, points.back());
		degree += 45.0f;
	}
	for (int k = 0; k < 8; k++)
	{
		filling.push_back(Triangle(points[k % 8].x, points[k % 8].y, points[(k + 1) % 8].x, points[(k + 1) % 8].y, 0.0f, 0.0f));
	}

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
	Initialize(points);

	glutMainLoop();

	// Cleaning
	glDeleteProgram(idProgram1);
	glDeleteVertexArrays(1, &idVBO_coord1);
	glDeleteVertexArrays(1, &idVAO1);
	glDeleteProgram(idProgram_2);
	glDeleteVertexArrays(1, &idVBO_coord_2);
	glDeleteVertexArrays(1, &idVAO_2);
	POINT_VECTOR = nullptr;
	TRIANGLE_FILLING = nullptr;
	return 0;
}
