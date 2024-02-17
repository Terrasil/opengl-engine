// -------------------------------------------------
// Programowanie grafiki 3D w OpenGL / UG
// -------------------------------------------------
#include <stdio.h>
#include <glew.h>
#include <freeglut.h>
#include <iostream>
#include <algorithm>
#define _USE_MATH_DEFINES
#include <cmath>
#include <time.h>

// Biblioteki GLM
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/matrix_decompose.hpp>
#include <gtx/string_cast.hpp>
#include <gtc/type_ptr.hpp>

// Lokalne pliki naglowkowe
#include "utilities.hpp"
#include "assets_repository.h"
#include "mesh.h"
#include "model.h"
#include "settings.h"
#include "renderer.h"


// Macierze PVM
glm::mat4 matProj;
glm::mat4 matView = glm::mat4(1.0);

std::vector<Model> models;

// Identyfikatory obiektow
//GLuint idVAO;

# define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

constexpr float one_degree = M_PI / 180.f;


AssetsRepository* assetsRepository = new AssetsRepository();
float getTranslationDistance(const glm::mat4& modelMatrix, glm::vec3 cameraPos) {
	glm::vec3 modelPos = ExtractCameraPos(modelMatrix);
	glm::vec3 tmp = cameraPos * glm::vec3(-2.0);
	return glm::distance(tmp, modelPos);
}

bool compareModels(Model& a, Model& b, glm::vec3 cameraPos) {
	//printf("COMPARE:\n");
	if (a.getMesh()->getTexture()->getFormat() == GL_RGBA && b.getMesh()->getTexture()->getFormat() != GL_RGBA) {
		return false;
	}
	if (a.getMesh()->getTexture()->getFormat() != GL_RGBA && b.getMesh()->getTexture()->getFormat() == GL_RGBA) {
		return true;
	}
	else {
		return getTranslationDistance(a.getMatrix(), cameraPos) > getTranslationDistance(b.getMatrix(), cameraPos);
	}
}
// ---------------------------------------------------
void DisplayScene()
{
	// Czyszczenie bufora koloru i glebokosci
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	__CHECK_FOR_ERRORS

	// ------------------------------------------------------------
	// Wyliczanie macierz widoku
	// ------------------------------------------------------------
	matView = UpdateViewMatrix();

	// -----------------------------------------------
	// NOWE : wyliczanie pozycji kamery z matView
	// -----------------------------------------------
	glm::vec3 cameraPos = ExtractCameraPos(matView);
	//std::sort(models.begin(), models.end(), compareModels(cameraPos));
	std::sort(models.begin(), models.end(), [cameraPos](Model& a, Model& b) {
		return compareModels(a, b, cameraPos);
	});
	for (Model model : models) {
		model.draw(cameraPos, matProj, matView); 
	}
	for (Light* light : Renderer::getInstance().getLights()) {
		light->draw(cameraPos, matProj, matView);
	}
	glDepthMask(GL_TRUE);
	
	glBindVertexArray(0);

	// Wylaczanie
	glUseProgram(0);


	glutSwapBuffers();

}

// ---------------------------------------------------
void Initialize()
{
	// Ustawienia globalne
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);

	glEnable(GL_DEPTH_TEST); 
	glDepthFunc(GL_LESS);

	glEnable(GL_BLEND); 
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	glEnable(GL_ALPHA_TEST);
	glAlphaFunc(GL_GREATER, 0.5);

	stbi_set_flip_vertically_on_load(true);
	glPixelStorei(GL_UNPACK_ALIGNMENT, 1); 

	// ------------------------------------------------------------
	// Ustawienie domyslnego odsuniecia kamery od polozenia (0,0,0)
	// ------------------------------------------------------------
	CameraTranslate_x = 0.0;
	CameraTranslate_y = -1.0;
	CameraTranslate_z = -12.0;



	// Wczytanie pliku obj
	assetsRepository->registerMesh("scene", "models/scene.obj");
	Mesh* mesh = assetsRepository->getMesh("scene");
	assetsRepository->registerMesh("sphere", "models/sphere.obj");
	Mesh* sphereMesh = assetsRepository->getMesh("sphere");
	assetsRepository->registerMesh("cube", "models/cube.obj");
	Mesh* cubeMesh = assetsRepository->getMesh("cube");
	assetsRepository->registerMesh("monkey", "models/monkey.obj");
	Mesh* monkeyMesh = assetsRepository->getMesh("monkey");

	assetsRepository->registerProgram("base");
	Program* program = assetsRepository->getProgram("base");
	assetsRepository->registerProgram("light");
	Program* program2 = assetsRepository->getProgram("light");

	assetsRepository->registerTexture("grid", "textures/uvtemplate.jpg", GL_RGB);
	Texture* texture = assetsRepository->getTexture("grid");
	assetsRepository->registerTexture("flower", "textures/flower32bit.png", GL_RGBA);
	Texture* texture2 = assetsRepository->getTexture("flower");
	assetsRepository->registerTexture("background", "textures/background.png", GL_RGBA);
	Texture* texture3 = assetsRepository->getTexture("background");
	Material* material = new Material();
	material->setAmbient(glm::vec3(0.2, 0.2, 0.2));
	material->setDiffuse(glm::vec3(1.0, 1.0, 1.0));
	material->setSpecular(glm::vec3(2.0, 2.0, 2.0));
	material->setShininess(32.0f);

	Model* sphere = new Model();
	sphere->setMatrix(glm::mat4(1.0));
	sphereMesh->setTexture(texture);
	sphere->setMesh(sphereMesh);
	sphere->setMaterial(material);
	sphere->setProgram(program2);

	Light* light = new Light();
	light->setAmbient(glm::vec3(0.1, 0.1, 0.1));
	light->setDiffuse(glm::vec3(0.11, 0.0, 0.0));
	light->setSpecular(glm::vec3(1.0, 1.0, 1.0));
	light->setAttenuation(glm::vec3(1.0, 0.0, 1.0));
	light->setPosition(glm::vec3(2.0, 3.0, 1.0));
	light->setIsDirectional(true);
	light->setModel(sphere);
	Renderer::getInstance().addLights(light);

	Model* sphere2 = new Model();
	sphere2->setMatrix(glm::mat4(1.0));
	sphereMesh->setTexture(texture);
	sphere2->setMesh(sphereMesh);
	sphere2->setMaterial(material);
	sphere2->setProgram(program2);

	Light* light2 = new Light();
	light2->setAmbient(glm::vec3(0.1, 0.1, 0.1));
	light2->setDiffuse(glm::vec3(0.0, 1.0, 1.0));
	light2->setSpecular(glm::vec3(1.0, 1.0, 1.0));
	light2->setAttenuation(glm::vec3(1.0, 0.0, 1.0));
	light2->setPosition(glm::vec3(1.0, 2.0, 0.0));
	light2->setIsDirectional(false);
	light2->setModel(sphere2);
	Renderer::getInstance().addLights(light2);

	Model cube;
	glm::mat4 cubeMat = glm::translate(glm::mat4(1.0), glm::vec3(-1.0, 2.0, 1.0));
	cube.setMatrix(cubeMat);
	cubeMesh->setTexture(texture3);
	cube.setMesh(cubeMesh);
	cube.setMaterial(material);
	cube.setProgram(program);
	cube.name = "cube";
	models.push_back(cube);

	Model monkey;
	glm::mat4 monkeyMat = glm::translate(glm::mat4(1.0), glm::vec3(3.0, 2.0, 1.0));
	monkeyMat = glm::scale(monkeyMat, glm::vec3(0.5, 0.5, 0.5));
	monkey.setMatrix(monkeyMat);
	monkeyMesh->setTexture(texture);
	monkey.setMesh(monkeyMesh);
	monkey.setMaterial(material);
	monkey.setProgram(program);
	monkey.name = "monkey";
	models.push_back(monkey);

	mesh->setTexture(texture);
	// Potok
	glAttachShader(program->getId(), LoadShader(GL_VERTEX_SHADER, "vertex.glsl"));
	glAttachShader(program->getId(), LoadShader(GL_FRAGMENT_SHADER, "fragment.glsl"));
	LinkAndValidateProgram(program->getId());
	glAttachShader(program2->getId(), LoadShader(GL_VERTEX_SHADER, "vertexLight.glsl"));
	glAttachShader(program2->getId(), LoadShader(GL_FRAGMENT_SHADER, "fragmentLight.glsl"));
	LinkAndValidateProgram(program2->getId());

	Model scene;
	scene.setMatrix(glm::mat4(1.0));
	scene.setMesh(mesh);
	scene.setProgram(program);
	scene.setMaterial(material);
	models.push_back(scene);

	// Wczytanie mesha trawy z pliku nagłówkowego
	Material* material2 = new Material();
	material2->setAmbient(glm::vec3(0.3, 0.3, 0.3));
	material2->setDiffuse(glm::vec3(1.0, 1.0, 1.0));
	material2->setSpecular(glm::vec3(0.5, 0.5, 0.5));
	material2->setShininess(2.0f);

	// Generowanie trawy w randomowych pozycjach

	assetsRepository->registerMesh("planeGrass", "models/planeGrass.obj");
	Mesh* planeGrass = assetsRepository->getMesh("planeGrass");
	planeGrass->setTexture(texture2);
	std::srand(static_cast<unsigned int>(time(NULL)));
	for (int i = 0; i < 100; ++i) {
		glm::mat4 mat = glm::mat4(1.0);
		mat = glm::translate(mat, glm::vec3(
			(std::rand() % 11) - 5.5, 0.5, (std::rand() % 11) - 5.5
		)); 
		mat = glm::scale(mat, glm::vec3(0.5, 0.5, 0.5));
		Model grass;
		scene.setMaterial(material2);
		grass.setMatrix(mat);
		grass.setMesh(planeGrass);
		grass.setProgram(program);
		models.push_back(grass);
	}
}

void FrameCallback(int i)
{
	glutPostRedisplay();
	for (Light* light : Renderer::getInstance().getLights()) {
		if (Settings::getInstance().animateLight) {
			light->setPosition(glm::vec3(2.0 * sin(time(NULL)), 3.0, 1.0 * cos(time(NULL))));
		}
		else {
			light->setPosition(glm::vec3(2.0, 3.0, 1.0));
		}
	}
	for (Model& model : models) {
		if (model.name == "cube") {
			glm::mat4 x = model.getMatrix();
			glm::mat4 y = glm::rotate(x, one_degree / 2, glm::vec3(0.0, 1.0, 0.0));
			model.setMatrix(y);
		}
		else if (model.name == "monkey") {
			glm::mat4 x = model.getMatrix();
			glm::mat4 y = glm::rotate(x, one_degree / 2, glm::vec3(0.0, 1.0, 0.0));
			glm::mat4 z = glm::translate(x, glm::vec3(sin(time(NULL))/10, 0.0, cos(time(NULL)) / 10));
			model.setMatrix(z);
		}
		else if (model.name == "sphere") {
			glm::mat4 x = glm::translate(glm::mat4(1.0), Renderer::getInstance().getLights()[0]->getPosition());
			x = glm::scale(x, glm::vec3(0.25, 0.25, 0.25));
			model.setMatrix(x);
		}
	}
	glutTimerFunc(1000.f / 60, FrameCallback, 0);
}

// ---------------------------------------------------
void Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	matProj = glm::perspective(glm::radians(90.0f), width / (float)height, 0.01f, 50.0f);
}

// ---------------------------------------------------
void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
	case 27:	// ESC key
		glutLeaveMainLoop();
		break;

	}
}
void processMenu(int option) {
	switch (option) {
	case 1:
		Settings::getInstance().lightVisibility = false;
		break;
	case 2:
		Settings::getInstance().lightVisibility = true;
		break;
	case 3:
		Settings::getInstance().lightMode = false;
		break;
	case 4:
		Settings::getInstance().lightMode = true;
		break;
	case 5:
		Settings::getInstance().lightQuality = false;
		break;
	case 6:
		Settings::getInstance().lightQuality = true;
		break;
	case 7:
		Settings::getInstance().animateLight = false;
		break;
	case 8:
		Settings::getInstance().animateLight = true;
		break;
		// Add more cases for additional menu items if needed
	}
	glutPostRedisplay();
}
void createMenu() {
	int lightVisibilityMenu = glutCreateMenu(processMenu);
	glutAddMenuEntry("Off", 1);
	glutAddMenuEntry("On", 2);

	int lightModeMenu = glutCreateMenu(processMenu);
	glutAddMenuEntry("Phong", 3);
	glutAddMenuEntry("Blinn-Phong", 4);

	int lightQualityMenu = glutCreateMenu(processMenu);
	glutAddMenuEntry("Low", 5);
	glutAddMenuEntry("High", 6);

	int lightAnimationMenu = glutCreateMenu(processMenu);
	glutAddMenuEntry("Disable", 7);
	glutAddMenuEntry("Enable", 8);

	int light = glutCreateMenu(processMenu);
	glutAddSubMenu("Visibility", lightVisibilityMenu);
	glutAddSubMenu("Mode", lightModeMenu);
	glutAddSubMenu("Quality", lightQualityMenu);
	glutAddSubMenu("Animation", lightAnimationMenu);

	int menu = glutCreateMenu(processMenu);
	glutAddSubMenu("Light", light);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// ---------------------------------------------------
int main(int argc, char* argv[])
{
	// GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitWindowSize(600, 500);
	glutCreateWindow("Programownie grafiki 3D w OpenGL");

	glutDisplayFunc(DisplayScene);
	glutReshapeFunc(Reshape);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutMouseWheelFunc(MouseWheel);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeys);
	glutTimerFunc(1000.0 / 60.0, FrameCallback, 0);
	createMenu();




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

	// Kod wykonywany po wyjsciu z glownej petli

	// Cleaning
	//glDeleteProgram(idProgram);
	//glDeleteVertexArrays(1, &idVAO);


	return 0;
}
