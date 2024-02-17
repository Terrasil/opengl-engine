#include <stdio.h>
#include <vector>
#include <ctime>
#include <functional>

#include <glew.h>
#include <freeglut.h>

#include "shader_stuff.h"


// ---------------------------------------
// Identyfikatory obiektow OpenGL

GLuint idProgram_1;	// programu
GLuint idVAO_1;		// tablic wierzcholkow
GLuint idVBO_coord_1; // bufora na wspolrzedne

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



// ---------------------------------------
void DisplayScene()
{
	// ---------------------------
	// Etap (5) rendering
	// ---------------------------
	glClear(GL_COLOR_BUFFER_BIT);


	// Wlaczenie potoku
	glUseProgram(idProgram_1);

	// Generowanie obiektow na ekranie
	glBindVertexArray(idVAO_1);
	glDrawArrays(GL_TRIANGLES, 0, TRIANGLES_VECTOR->size()/2 * 3);
	glBindVertexArray(0);

	// Wylaczanie
	glUseProgram(0);


	glUseProgram(idProgram_2);

	// Generowanie obiektow na ekranie
	glBindVertexArray(idVAO_2);
	glDrawArrays(GL_TRIANGLES, 0, TRIANGLES_VECTOR->size() / 2 * 3);
	glBindVertexArray(1);

	// Wylaczanie
	glUseProgram(1);


	glutSwapBuffers();
}

// ---------------------------------------
void Initialize(std::vector<Triangle> &triangles)
{
	// -------------------------------------------------
	// Etap (2) przeslanie danych wierzcholków do OpenGL
	// -------------------------------------------------
	glGenVertexArrays(1, &idVAO_1);
	glBindVertexArray(idVAO_1);

	// Bufor na wspolrzedne wierzcholkow
	glGenBuffers(1, &idVBO_coord_1);
	glBindBuffer(GL_ARRAY_BUFFER, idVBO_coord_1);
	glBufferData(GL_ARRAY_BUFFER, (sizeof(Triangle) * triangles.size()) / 2, &(triangles[0]), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);


	// ---------------------------------------
	// Etap (3) stworzenie potoku graficznego
	// ---------------------------------------
	idProgram_1 = glCreateProgram();

	glAttachShader(idProgram_1, LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader(idProgram_1, LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));

	LinkAndValidateProgram(idProgram_1);
/////////////////////////////////////////////////////////////////////////////////////////
	// Tworzenie dodatkowego potoku graficznego

	glGenVertexArrays(1, &idVAO_2);
	glBindVertexArray(idVAO_2);

	// Bufor na wspolrzedne wierzcholkow
	glGenBuffers(1, &idVBO_coord_2);
	glBindBuffer(GL_ARRAY_BUFFER, idVBO_coord_2);
	glBufferData(GL_ARRAY_BUFFER, (sizeof(Point) * 3 * triangles.size()) / 2, &(triangles[triangles.size()/2]), GL_STATIC_DRAW);

	glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	glBindVertexArray(0);
	idProgram_2 = glCreateProgram();

	glAttachShader(idProgram_2, LoadShader(GL_VERTEX_SHADER, "vertex-2.glsl"));
	glAttachShader(idProgram_2, LoadShader(GL_FRAGMENT_SHADER, "fragment-2.glsl"));

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
	// Etap (1) utworzynie kontektu i okna aplikacji
	// -----------------------------------------------

	// Data
	srand(time(NULL));
	std::vector<Triangle> triangles;
	TRIANGLES_VECTOR = &triangles;
	for (int i = 0; i < 200; i++)
		triangles.push_back(Triangle(randomFloat(), randomFloat(), randomFloat(), randomFloat(), randomFloat(), randomFloat()));


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
	Initialize(triangles);



	glutMainLoop();


	// Cleaning
	glDeleteProgram(idProgram_1);
	glDeleteVertexArrays(1, &idVBO_coord_1);
	glDeleteVertexArrays(1, &idVAO_1);
	glDeleteProgram(idProgram_2);
	glDeleteVertexArrays(1, &idVBO_coord_2);
	glDeleteVertexArrays(1, &idVAO_2);
	TRIANGLES_VECTOR = nullptr;
	return 0;
}
