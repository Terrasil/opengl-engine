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

#include "utilities.hpp"
#include "obj_loader.hpp"
# define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"


// ---------------------------------------
// Identyfikatory obiektow OpenGL
class Program {
public:
	GLuint id = NULL;
	GLuint VAO = NULL;
	GLuint coordVBO = NULL;
	GLuint uvVBO = NULL;
	GLuint colorVBO = NULL;
	GLuint texture = NULL;
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
int ticks = 0;
constexpr float one_degree = M_PI / 180.f;

// ---------------------------------------
void DisplayScene()
{
	// ---------------------------
	// Etap (5) rendering
	// ---------------------------
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// Wlaczenie potoku
	for (Model model : models){
		glUseProgram(model.program->id); 
		glUniformMatrix4fv(glGetUniformLocation(model.program->id, "matProj"), 1, GL_FALSE, glm::value_ptr(matProj));
		glUniformMatrix4fv(glGetUniformLocation(model.program->id, "matView"), 1, GL_FALSE, glm::value_ptr(matView));
		glUniformMatrix4fv(glGetUniformLocation(model.program->id, "matModel"), 1, GL_FALSE, glm::value_ptr(model.matrix));
		glUniform1i(glGetUniformLocation(model.program->id, "time"), ticks);
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, model.program->texture);
		glUniform1i(glGetUniformLocation(model.program->id, "tex"), 0);
		glBindVertexArray(model.program->VAO);
		glDrawArrays(GL_TRIANGLES, 0, model.vertices.size());
	}
	// Wylaczanie
	glBindVertexArray(0);
	glUseProgram(0);
	ticks++;
	glutSwapBuffers();
}

// ---------------------------------------
void Initialize()
{
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	Model cube;
	cube.program->id = glCreateProgram();
	cube.load("src/modified_cube.obj");
	models.push_back(cube);


	GLuint vertexShader = LoadShader(GL_VERTEX_SHADER, "vertex.glsl");
	GLuint fragmentShader = LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl");

	// ------------------------------------
	// NOWE: Wczytanie pliku tekstury
	// ------------------------------------

	int tex_width, tex_height, tex_n;
	unsigned char* tex_data;

	// tylko raz w calym programie
	stbi_set_flip_vertically_on_load(true);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1);

	// wczytywanie pliku graficznego
	tex_data = stbi_load("background.png", &tex_width, &tex_height, &tex_n, 0);
	if (tex_data == NULL) {
		printf("Image can’t be loaded!\n");
	}

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

		glGenBuffers(1, &(model.program->colorVBO));
		glBindBuffer(GL_ARRAY_BUFFER, model.program->colorVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * model.normals.size(), &(model.normals.at(0)), GL_STATIC_DRAW);
		glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(1);

		glGenBuffers(1, &(model.program->uvVBO));
		glBindBuffer(GL_ARRAY_BUFFER, model.program->uvVBO);
		glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * model.uvs.size(), &(model.uvs.at(0)), GL_STATIC_DRAW);
		glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
		glEnableVertexAttribArray(2);

		glGenTextures(1, &(model.program->texture));
		glBindTexture(GL_TEXTURE_2D, model.program->texture);
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, tex_width, tex_height, 0, GL_RGBA, GL_UNSIGNED_BYTE, tex_data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
	glBindVertexArray(0);
	matView = glm::mat4(1.0);
	matView = glm::translate(matView, glm::vec3(0, 0, -6));
	matView = glm::rotate(matView, one_degree * 10, glm::vec3(1, 0, 0));

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
	matView = glm::rotate(matView, one_degree / 2, glm::vec3(0, 1, 0));
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
	glutCreateWindow("Szablon programu w OpenGL - Lab 4 - Zadanie 4 A");
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
