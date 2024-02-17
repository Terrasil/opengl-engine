#include <stdio.h>
#include <vector>
#include <functional>
#include <iostream>

#include <glew.h>
#include <freeglut.h>

#include "shader_stuff.h"
#include "house.h"


// ---------------------------------------
// Identyfikatory obiektow OpenGL

GLuint idProgram;	// programu
GLuint idVAO;		// tablic wierzcholkow
GLuint idVBO_coord; // bufora na wspolrzedne
int frames = 0;

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

class Triangle
{
public:
	Point p1;
	Point p2;
	Point p3;

	Triangle(float x1, float y1, float x2, float y2, float x3, float y3)
		: p1(x1, y1), p2(x2, y2), p3(x3, y3) {}
};

std::vector<Triangle>* TRIANGLES_VECTOR = nullptr;


void RotateTriangle(float RadianRotation, Triangle &tri)
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
	glUseProgram(idProgram);

	int timeLocation = glGetUniformLocation(idProgram, "time");

	glUniform1f(timeLocation, ((float) frames )/ 60.0f);

	// Generowanie obiektow na ekranie
	glBindVertexArray(idVAO);
	glDrawArrays(GL_TRIANGLES, 0, TRIANGLES_VECTOR->size() * 3);
	glBindVertexArray(0);


	// Wylaczanie
	glUseProgram(0);

	frames++;
	glutSwapBuffers();
}

// ---------------------------------------
void Initialize(std::vector<Triangle> &triangles)
{
	// -------------------------------------------------
	// Etap (2) przeslanie danych wierzcholków do OpenGL
	// -------------------------------------------------
	glGenVertexArrays(1, &idVAO);
	glBindVertexArray(idVAO);

	// Bufor na wspolrzedne wierzcholkow
	glGenBuffers(1, &idVBO_coord);
	glBindBuffer(GL_ARRAY_BUFFER, idVBO_coord);
	glBufferData(GL_ARRAY_BUFFER, sizeof(Triangle)*triangles.size(), &(triangles[0]), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);


	// ---------------------------------------
	// Etap (3) stworzenie potoku graficznego
	// ---------------------------------------
	idProgram = glCreateProgram();

	glAttachShader(idProgram, LoadShader(GL_VERTEX_SHADER, "vertex1.glsl"));
	glAttachShader(idProgram, LoadShader(GL_FRAGMENT_SHADER, "fragment1.glsl"));

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

int frameDelay = 1000 / 30; // 30 frames per second
void timer(int value) {
	glutPostRedisplay(); // Request a display callback
	glutTimerFunc(frameDelay, timer, 0); // Set the timer for the next frame
}

// ---------------------------------------------------
int main(int argc, char* argv[])
{
	// -----------------------------------------------
	// Etap (1) utworzynie kontektu i okna aplikacji
	// -----------------------------------------------

	// Data
	srand(time(NULL));
	std::vector<Triangle> triangles;
	TRIANGLES_VECTOR = &triangles;
	for (int i = 0; i < (NUMBER_OF_TRIANGLES * 6); i+=6)
	{
		triangles.push_back(Triangle(Mesh_Vertices[i], Mesh_Vertices[i+1], Mesh_Vertices[i+2], Mesh_Vertices[i+3], Mesh_Vertices[i+4], Mesh_Vertices[i+5]));
		RotateTriangle(-90.0f, triangles.back());
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
	glutTimerFunc(0, timer, 0); // Start the timer


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
