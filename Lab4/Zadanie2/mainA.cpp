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

#include "obj_loader.hpp"
#include "utilities.hpp"


// ---------------------------------------
// Identyfikatory obiektow OpenGL
class Program {
public:
	GLuint id = NULL;
	GLuint VAO = NULL;
	GLuint coordVBO = NULL;
	GLuint uvVBO = NULL;
	GLuint normalVBO = NULL;
	Program() {};
};
class Model {
public:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	Program* program = new Program();
	glm::mat4 matrix = glm::mat4(1.0);
	void load(std::string path) {
		if (!loadOBJ(path.c_str(), vertices, uvs, normals)) {
			std::cout << "Can't load obj file!" << std::endl;
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
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	//matView = UpdateViewMatrix();
	glm::vec3 cameraPos = ExtractCameraPos(matView);

	int modelIndex = 0;
	// Wlaczenie potoku
	for (Model model : models){
		glUseProgram(model.program->id);
		glUniform3fv(glGetUniformLocation(model.program->id, "cameraPos"), 1, &cameraPos[0]);
		glUniformMatrix4fv(glGetUniformLocation(model.program->id, "matProj"), 1, GL_FALSE, glm::value_ptr(matProj));
		glUniformMatrix4fv(glGetUniformLocation(model.program->id, "matView"), 1, GL_FALSE, glm::value_ptr(matView));
		glUniformMatrix4fv(glGetUniformLocation(model.program->id, "matModel"), 1, GL_FALSE, glm::value_ptr(model.matrix));
		glBindVertexArray(model.program->VAO);
		glDrawArrays(GL_TRIANGLES, 0, model.vertices.size());
		if (modelIndex > 0 && modelIndex < 5) {
			models[modelIndex].matrix = glm::scale(models[modelIndex].matrix, glm::vec3(1, 0.5, 1));
			models[modelIndex].matrix = glm::translate(models[modelIndex].matrix, glm::vec3(0, -2, 0));
			models[modelIndex].matrix = glm::rotate(models[modelIndex].matrix, one_degree, glm::vec3(0, 0, 1));
			models[modelIndex].matrix = glm::translate(models[modelIndex].matrix, glm::vec3(0, 2, 0));
			models[modelIndex].matrix = glm::scale(models[modelIndex].matrix, glm::vec3(1, 2, 1));
		}
		else {
			models[modelIndex].matrix = glm::rotate(models[modelIndex].matrix, one_degree, glm::vec3(0, 1, 0));
		}
		modelIndex++;
	}
	// Wylaczanie
	glBindVertexArray(0);
	glUseProgram(0);

	glutSwapBuffers();
}

// ---------------------------------------
void Initialize()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	Model monkey;
	monkey.program->id = glCreateProgram();
	monkey.load("src/monkey.obj");
	models.push_back(monkey);

	Model cone1;
	cone1.program->id = glCreateProgram();
	cone1.load("src/cone.obj");
	cone1.matrix = glm::translate(cone1.matrix, glm::vec3(2, 0, 0));
	cone1.matrix = glm::rotate(cone1.matrix, 90 * one_degree, glm::vec3(0, 0, -1));
	cone1.matrix = glm::scale(cone1.matrix, glm::vec3(1, 2, 1));
	models.push_back(cone1);

	Model cone2;
	cone2.program->id = glCreateProgram();
	cone2.load("src/cone.obj");
	cone2.matrix = glm::translate(cone2.matrix, glm::vec3(-2, 0, 0));
	cone2.matrix = glm::rotate(cone2.matrix, 90 * one_degree, glm::vec3(0, 0, 1));
	cone2.matrix = glm::scale(cone2.matrix, glm::vec3(1, 2, 1));
	models.push_back(cone2);

	Model cone3;
	cone3.program->id = glCreateProgram();
	cone3.load("src/cone.obj");
	cone3.matrix = glm::scale(cone3.matrix, glm::vec3(1, 2, 1));
	cone3.matrix = glm::rotate(cone3.matrix, 90 * one_degree, glm::vec3(0, 0, 1));
	cone3.matrix = glm::translate(cone3.matrix, glm::vec3(-1, 0, 0));
	cone3.matrix = glm::rotate(cone3.matrix, 90 * one_degree, glm::vec3(0, 0, 1));
	models.push_back(cone3);

	Model cone4;
	cone4.program->id = glCreateProgram();
	cone4.load("src/cone.obj");
	cone4.matrix = glm::scale(cone4.matrix, glm::vec3(1, 2, 1));
	cone4.matrix = glm::rotate(cone4.matrix, 90 * one_degree, glm::vec3(0, 0, -1));
	cone4.matrix = glm::translate(cone4.matrix, glm::vec3(-1, 0, 0));
	cone4.matrix = glm::rotate(cone4.matrix, 90 * one_degree, glm::vec3(0, 0, 1));
	models.push_back(cone4);

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
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * model.vertices.size(), &(model.vertices.at(0)), GL_STATIC_DRAW);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(0);

		glGenBuffers(1, &(model.program->uvVBO));
		glBindBuffer(GL_ARRAY_BUFFER, model.program->uvVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * model.uvs.size(), &(model.uvs.at(0)), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &(model.program->normalVBO));
		glBindBuffer(GL_ARRAY_BUFFER, model.program->normalVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * model.normals.size(), &(model.normals.at(0)), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(2);
	}
	glBindVertexArray(0);
	matView = glm::mat4(1.0);
	matView = glm::translate(matView, glm::vec3(0, 0, -10));
	matView = glm::rotate(matView, one_degree * 45, glm::vec3(1, 0, 0));

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

void FrameCallback(int i)
{
	glutPostRedisplay();
	//matView = glm::rotate(matView, one_degree / 2, glm::vec3(0, 1, 0));
	glutTimerFunc(1000.f / 60, FrameCallback, 0);
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
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitWindowSize(500, 500);
	glutCreateWindow("Szablon programu w OpenGL - Lab 4 - Zadanie 2 A");
	glutDisplayFunc(DisplayScene);
	glutReshapeFunc(Reshape);
	glutKeyboardFunc(Keyboard);
	glutTimerFunc(1000.0 / 60.0, FrameCallback, 0);


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
