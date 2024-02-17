#define _USE_MATH_DEFINES

#include <stdio.h>
#include <vector>
#include <ctime>
#include <iostream>
#include <functional>
#include <string>

#include <cmath>
#include <glew.h>
#include <freeglut.h>
#include <glm.hpp>
#include <array>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include "shader_stuff.h"
#include "obiektA.h"
#include "obiektB.h"


// ---------------------------------------
// Identyfikatory obiektow OpenGL
class Program {
public:
	GLuint id = NULL;
	GLuint VAO = NULL;
	GLuint coordVBO = NULL;
	Program() {};
};
class Model {
public:
	std::vector<glm::vec2> polygons;
	Program* program = new Program();
	glm::mat4 matrix = glm::mat4(1.0);
	void load(GLfloat coord[], GLuint indices[], int coordSize, int indicesSize) {
		for (int i = 0; i < indicesSize; i++)
		{
			polygons.push_back(
				glm::vec2(coord[indices[i] * 2], coord[indices[i] * 2 + 1])
			);
		}
	}
	Model() {};
};
glm::mat4 matProj;
glm::mat4 matView;
GLfloat fov = 3.f;
std::vector<Model> models;
constexpr float one_degree = M_PI / 180.f;


// ---------------------------------------
void DisplayScene()
{
	// ---------------------------
	// Etap (5) rendering
	// ---------------------------
	glClear(GL_COLOR_BUFFER_BIT);


	// Wlaczenie potoku
	for (Model model : models){
		glUseProgram(model.program->id); 
		glUniformMatrix4fv(glGetUniformLocation(model.program->id, "matProj"), 1, GL_FALSE, glm::value_ptr(matProj));
		glUniformMatrix4fv(glGetUniformLocation(model.program->id, "matView"), 1, GL_FALSE, glm::value_ptr(matView));
		glUniformMatrix4fv(glGetUniformLocation(model.program->id, "matModel"), 1, GL_FALSE, glm::value_ptr(model.matrix));
		glBindVertexArray(model.program->VAO);
		glDrawArrays(GL_TRIANGLES, 0, model.polygons.size());
	}
	// Wylaczanie
	glBindVertexArray(0);
	glUseProgram(0);


	glutSwapBuffers();
}

// ---------------------------------------
void Initialize()
{
	Model obiektA;
	obiektA.program->id = glCreateProgram();
	obiektA.load(obiektA::coord, obiektA::idx, sizeof(obiektA::coord) / sizeof(GLfloat) , sizeof(obiektA::idx) / sizeof(GLuint));
	obiektA.matrix = glm::translate(obiektA.matrix, glm::vec3(-1, 0, 0));
	models.push_back(obiektA);
	Model obiektB1;
	obiektB1.program->id = glCreateProgram();
	obiektB1.load(obiektB::coord, obiektB::idx, sizeof(obiektB::coord) / sizeof(GLfloat), sizeof(obiektB::idx) / sizeof(GLuint));
	obiektB1.matrix = glm::translate(obiektB1.matrix, glm::vec3(0, 0, 0));
	models.push_back(obiektB1);
	Model obiektB2;
	obiektB2.program->id = glCreateProgram();
	obiektB2.load(obiektB::coord, obiektB::idx, sizeof(obiektB::coord) / sizeof(GLfloat), sizeof(obiektB::idx) / sizeof(GLuint));
	obiektB2.matrix = glm::translate(obiektB2.matrix, glm::vec3(0, 1, 0));
	models.push_back(obiektB2);

	GLuint vertexShader = LoadShader(GL_VERTEX_SHADER, "vertex.glsl");
	GLuint fragmentShader = LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl");
	for (Model model : models){
		glAttachShader(model.program->id, vertexShader);
		glAttachShader(model.program->id, fragmentShader);
		LinkAndValidateProgram(model.program->id);
		glGenVertexArrays(1, &(model.program->VAO));
		glBindVertexArray(model.program->VAO);

		glGenBuffers(1, &(model.program->coordVBO));
		glBindBuffer(GL_ARRAY_BUFFER, model.program->coordVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * model.polygons.size(), &(model.polygons.at(0)), GL_STATIC_DRAW);

		glVertexAttribPointer(0, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);
	}
	glBindVertexArray(0);
	matView = glm::mat4(1.0);
	matView = glm::translate(matView, glm::vec3(0, 0, -4));

	// ustawienie koloru czyszczenia ramki koloru
	glClearColor(0.9f, 0.9f, 0.9f, 0.9f);

}

// ---------------------------------------
// Funkcja wywolywana podczas zmiany rozdzielczosci ekranu
void Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	matProj = glm::perspective(3.1415 / (fov), (double)width / (double)height, 0.1, 60.0);
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
	case 'w':
		models[1].matrix = glm::scale(models[1].matrix, glm::vec3(1.1, 1.1, 1));
		break;
	case 's':
		models[1].matrix = glm::scale(models[1].matrix, glm::vec3(0.9, 0.9, 1));
		break;
	case 'd':
		models[0].matrix = glm::rotate(models[0].matrix, one_degree * 2, glm::vec3(0, 0, 1));
		break;
	case 'a':
		models[0].matrix = glm::rotate(models[0].matrix, -one_degree * 2, glm::vec3(0, 0, 1));
		break;
	case ' ':	// Space key
		printf("SPACE!\n");
		// ponowne renderowanie
		glutPostRedisplay();
		break;
	}
	glutPostRedisplay();
}

void FrameCallback(int i)
{
	glutPostRedisplay();
	//matView = glm::rotate(matView, one_degree / 2, glm::vec3(0, 1, 0));
}

// ---------------------------------------------------
int main(int argc, char* argv[])
{
	// -----------------------------------------------
	// Etap (1) utworzynie kontektu k okna aplikacji
	// -----------------------------------------------

	// Data
	srand(time(NULL));

	// GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Szablon programu w OpenGL - Lab 3 - Zadanie 3");
	glutDisplayFunc(DisplayScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	//glutTimerFunc(1000.0 / 60.0, FrameCallback, 0);


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
	Initialize();

	glutMainLoop();

	// Cleaning
	//glDeleteProgram(idProgram);
	//glDeleteVertexArrays(1, &idVBO_coord);
	//glDeleteVertexArrays(1, &idVAO);

	return 0;
}
