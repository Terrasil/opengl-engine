// -------------------------------------------------
// Programowanie grafiki 3D w OpenGL / UG
// -------------------------------------------------
#include <stdio.h>
#include <glew.h>
#include <freeglut.h>
#include <iostream>
#include <algorithm>
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
#include "skybox.h"
#include "multimesh.h"
#include "sprite.h"

// Macierze PVM
glm::mat4 matProj;
glm::mat4 minimapProj;
glm::mat4 lightProj;
glm::mat4 matView = glm::mat4(1.0);

std::vector<Model> models;
std::vector<Sprite> sprites;


constexpr float one_degree = 3.14 / 180.f;

Material* testMultiMeshMaterial = new Material();
AssetsRepository* assetsRepository = new AssetsRepository();

GLuint minimapFBO;
GLuint minimapTexture;
GLuint minimapDepth;
GLuint minimapRBO;
GLuint postprocessFBO;
GLuint postprocessTexture;
GLuint postprocessDepth;
GLuint postprocessRBO;
GLuint shadowFBO;
GLuint shadowTexture;

Texture* postProcessTexture = new Texture();
Texture* miniMapTexture = new Texture();

// ---------------------------------------------------
void DisplayScene()
{
	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	// Renderowanie do postprocessu
	glBindFramebuffer(GL_FRAMEBUFFER, postprocessFBO);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	// ------------------------------------------------------------
	// Wyliczanie macierz widoku
	// ------------------------------------------------------------
	matView = UpdateViewMatrix();

	// -----------------------------------------------
	// NOWE : wyliczanie pozycji kamery z matView
	// -----------------------------------------------
	glm::vec3 cameraPos = ExtractCameraPos(matView);
	Renderer::getInstance().getSkyBoxes().at(Settings::getInstance().skyBox)->draw(matProj, matView);
	glm::mat4 multiMeshMatrix = glm::mat4(1.0);
	glm::vec3 multiMeshColor = glm::vec3(1.0);
	for (Model model : models) {
		model.draw(cameraPos, matProj, matView);
	}
	Renderer::getInstance().getMultiMesh()->draw(assetsRepository->getProgram("multimesh"), multiMeshMatrix, cameraPos, matProj, matView, testMultiMeshMaterial, multiMeshColor);
	for (Light* light : Renderer::getInstance().getLights()) {
		light->draw(cameraPos, matProj, matView);
	}
}
void Minimap()
{
	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	bool lightState = Settings::getInstance().lightVisibility;
	Settings::getInstance().lightVisibility = false;

	glBindFramebuffer(GL_FRAMEBUFFER, minimapFBO);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glm::vec3 cameraPos = ExtractCameraPos(matView);
	glm::mat4 matView = glm::lookAt(
		glm::vec3(cameraPos.x, 10, cameraPos.z),
		glm::vec3(cameraPos.x, 0, cameraPos.z),
		glm::vec3(0.0, 1.0, 1.0)
	);

	for (Model model : models) {
		model.draw(cameraPos, minimapProj, matView);
	}
	for (Light* light : Renderer::getInstance().getLights()) {
		light->draw(cameraPos, minimapProj, matView);
	}

	Settings::getInstance().lightVisibility = lightState;
}
void Shadows()
{
	glViewport(0, 0, 4096, 4096);
	bool lightQuality = Settings::getInstance().lightQuality;
	bool lightMode = Settings::getInstance().lightMode;
	bool lightVisibility = Settings::getInstance().lightVisibility;
	bool animateLight = Settings::getInstance().animateLight;
	bool fogVisibility = Settings::getInstance().fogVisibility;
	bool showRefraction = Settings::getInstance().showRefraction;
	bool showReflection = Settings::getInstance().showReflection;
	Settings::getInstance().lightVisibility = false;
	Settings::getInstance().lightQuality = false;
	Settings::getInstance().lightMode = false;
	Settings::getInstance().lightVisibility = false;
	Settings::getInstance().animateLight = false;
	Settings::getInstance().fogVisibility = false;
	Settings::getInstance().showRefraction = false;
	Settings::getInstance().showReflection = false;

	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glm::vec3 cameraPos = ExtractCameraPos(matView);
	glm::vec3 lightPosition = glm::vec3(2.0, 3.0, 1.0);
	glm::vec3 Light_Direction = glm::vec3(0.0, 0.0, 0.0);
	//glm::vec3 Light_Position = glm::vec3(0.0f, 5.0f, -25.0f);
	glm::mat4 lightView = glm::lookAt(lightPosition, Light_Direction, glm::vec3(0.0f, 1.0f, 0.0f));
	Renderer::getInstance().setLightView(lightView);
	for (Model model : models) {
		model.draw(cameraPos, lightProj, lightView);
	}

	Settings::getInstance().lightQuality = lightQuality;
	Settings::getInstance().lightMode = lightMode;
	Settings::getInstance().lightVisibility = lightVisibility;
	Settings::getInstance().animateLight = animateLight;
	Settings::getInstance().fogVisibility = fogVisibility;
	Settings::getInstance().showRefraction = showRefraction;
	Settings::getInstance().showReflection = showReflection;
}
void Render()
{
	// Czyszczenie bufora koloru i glebokosci
	glBindFramebuffer(GL_FRAMEBUFFER, 0);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);

	glEnable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	Minimap();
	Shadows();
	DisplayScene();

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glDisable(GL_DEPTH_TEST);

	if (Settings::getInstance().showMinimap) {
		for (Sprite sprite : sprites) {
			sprite.draw();
		}
	}

	glBindVertexArray(0);
	glUseProgram(0);

	glutSwapBuffers();
	glFlush();

}
// ---------------------------------------------------
void Initialize()
{
	// ------------------------------------------------------------
	// Framebuffer dla MINIMAPY
	// ------------------------------------------------------------
	glGenFramebuffers(1, &minimapFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, minimapFBO);

	glGenTextures(1, &minimapDepth);
	glBindTexture(GL_TEXTURE_2D, minimapDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, minimapDepth, 0);

	glGenTextures(1, &minimapTexture);
	glBindTexture(GL_TEXTURE_2D, minimapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, minimapTexture, 0);

	glGenRenderbuffers(1, &minimapRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, minimapRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	glFramebufferRenderbuffer(GL_RENDERBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, minimapRBO);
	
	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;

	// ------------------------------------------------------------
	// Framebuffer dla POSTPROCESU
	// ------------------------------------------------------------
	glGenFramebuffers(1, &postprocessFBO);
	glBindFramebuffer(GL_FRAMEBUFFER, postprocessFBO);

	glGenTextures(1, &postprocessDepth);
	glBindTexture(GL_TEXTURE_2D, postprocessDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, postprocessDepth, 0);

	glGenTextures(1, &postprocessTexture);
	glBindTexture(GL_TEXTURE_2D, postprocessTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, postprocessTexture, 0);

	glGenRenderbuffers(1, &postprocessRBO);
	glBindRenderbuffer(GL_RENDERBUFFER, postprocessRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	glFramebufferRenderbuffer(GL_RENDERBUFFER, GL_DEPTH_STENCIL_ATTACHMENT, GL_RENDERBUFFER, postprocessRBO);

	if (glCheckFramebufferStatus(GL_FRAMEBUFFER) != GL_FRAMEBUFFER_COMPLETE)
		std::cout << "ERROR::FRAMEBUFFER:: Framebuffer is not complete!" << std::endl;
	
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	// 1. Stworzenie obiektu tekstury
	glGenTextures(1, &shadowTexture);
	glBindTexture(GL_TEXTURE_2D, shadowTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 4096, 4096, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 0.0f, 0.0f, 0.0f, 1.0f };
	glTexParameterfv(GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, borderColor);

	// 2. Stworzenie obiektu ramki bufora jedynie z buforem glebokosci
	// (skladowa koloru nie jest nam potrzebna)
	glGenFramebuffers(1, &shadowFBO);

	// 3. Dolaczanie tekstury do ramki bufora
	glBindFramebuffer(GL_FRAMEBUFFER, shadowFBO);
	glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, shadowTexture, 0);
	glDrawBuffer(GL_NONE);
	glReadBuffer(GL_NONE);
	glBindFramebuffer(GL_FRAMEBUFFER, 0);


	Texture* shadowMapTexture = new Texture();
	shadowMapTexture->setId(shadowTexture);
	Renderer::getInstance().setShadowTexture(shadowMapTexture);

	// ------------------------------------------------------------
	// Ustawienie domyslnego odsuniecia kamery od polozenia (0,0,0)
	// ------------------------------------------------------------
	CameraTranslate_x = 0.0;
	CameraTranslate_y = -1.0;
	CameraTranslate_z = -12.0;

	testMultiMeshMaterial->setAmbient(glm::vec3(0.3, 0.3, 0.3));
	testMultiMeshMaterial->setDiffuse(glm::vec3(1.0, 1.0, 1.0));
	testMultiMeshMaterial->setSpecular(glm::vec3(0.5, 0.5, 0.5));
	testMultiMeshMaterial->setShininess(2.0f);
	testMultiMeshMaterial->setReflection(1.0f);
	// Wczytanie pliku obj
	assetsRepository->registerMesh("scene", "models/scene.obj");
	Mesh* mesh = assetsRepository->getMesh("scene");
	assetsRepository->registerMesh("sphere", "models/sphere.obj");
	Mesh* sphereMesh = assetsRepository->getMesh("sphere");
	assetsRepository->registerMesh("cube", "models/cube.obj");
	Mesh* cubeMesh = assetsRepository->getMesh("cube");
	assetsRepository->registerMesh("monkey", "models/monkey.obj");
	Mesh* monkeyMesh = assetsRepository->getMesh("monkey");

	MultiMesh* testMultiMesh = new MultiMesh();
	testMultiMesh->load("models/planeGrass.obj");
	assetsRepository->registerTexture("snowflake", "textures/snowflake.png", GL_RGBA);
	Texture* snowflake = assetsRepository->getTexture("snowflake");
	testMultiMesh->setTexture(snowflake);

	std::vector<glm::mat4x4> multiMeshMatrixes;
	std::srand(static_cast<unsigned int>(time(NULL)));
	for (int i = 0; i < 100000; ++i)
	{
		glm::mat4x4 multiMeshMatrix = glm::mat4x4(1.0);

		multiMeshMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(
			(std::rand() % 200) - 100, (std::rand() % 200) - 100, (std::rand() % 200) - 100
		));
		multiMeshMatrix = glm::rotate(multiMeshMatrix, (std::rand() % 11) - 5.5f, glm::vec3(0.0f, 1.0f, 0.0f));
		multiMeshMatrix = glm::scale(multiMeshMatrix, glm::vec3(0.1));

		multiMeshMatrixes.push_back(multiMeshMatrix);
	}
	testMultiMesh->setMatrixes(multiMeshMatrixes);
	Renderer::getInstance().setMultiMesh(testMultiMesh);

	assetsRepository->registerProgram("base");
	Program* program = assetsRepository->getProgram("base");
	assetsRepository->registerProgram("light");
	Program* program2 = assetsRepository->getProgram("light");
	assetsRepository->registerProgram("multimesh");
	Program* programMuliMesh = assetsRepository->getProgram("multimesh");
	assetsRepository->registerProgram("sprite");
	Program* programSprite = assetsRepository->getProgram("sprite");
	glAttachShader(programSprite->getId(), LoadShader(GL_VERTEX_SHADER, "shaders/sprite-vert.glsl"));
	glAttachShader(programSprite->getId(), LoadShader(GL_FRAGMENT_SHADER, "shaders/sprite-frag.glsl"));
	LinkAndValidateProgram(programSprite->getId());
	assetsRepository->registerProgram("postprocess");
	Program* programPostprocess = assetsRepository->getProgram("postprocess");
	glAttachShader(programPostprocess->getId(), LoadShader(GL_VERTEX_SHADER, "shaders/postprocess-vert.glsl"));
	glAttachShader(programPostprocess->getId(), LoadShader(GL_FRAGMENT_SHADER, "shaders/postprocess-frag.glsl"));
	LinkAndValidateProgram(programPostprocess->getId());
	assetsRepository->registerProgram("shadow");
	Program* programShadow = assetsRepository->getProgram("shadow");
	glAttachShader(programShadow->getId(), LoadShader(GL_VERTEX_SHADER, "shaders/depthmap-vertex.glsl"));
	glAttachShader(programShadow->getId(), LoadShader(GL_FRAGMENT_SHADER, "shaders/depthmap-fragment.glsl"));
	LinkAndValidateProgram(programShadow->getId());

	postProcessTexture->setId(postprocessTexture);
	Sprite postprocessSprite;
	postprocessSprite.setTexture(postProcessTexture);
	postprocessSprite.setPosition(glm::vec2(0.0, 0.0));
	postprocessSprite.setSize(glm::vec2(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)));
	postprocessSprite.setProgram(assetsRepository->getProgram("postprocess"));
	postprocessSprite.setName("postprocess");
	sprites.push_back(postprocessSprite);
	miniMapTexture->setId(minimapTexture); //MINIMAPTEXTURE
	Sprite minimapSprite;
	minimapSprite.setTexture(miniMapTexture);
	minimapSprite.setPosition(glm::vec2(10.0,10.0));
	minimapSprite.setSize(glm::vec2(200.0, 200.0));
	minimapSprite.setProgram(assetsRepository->getProgram("sprite"));
	minimapSprite.setName("minimap");
	sprites.push_back(minimapSprite);


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
	light->setDiffuse(glm::vec3(0.3, 0.3, 0.28));
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
	light2->setDiffuse(glm::vec3(1.0, 0.0, 0.0));
	light2->setSpecular(glm::vec3(1.0, 1.0, 1.0));
	light2->setAttenuation(glm::vec3(1.0, 0.0, 1.0));
	light2->setPosition(glm::vec3(1.0, 2.0, 0.0));
	light2->setIsDirectional(false);
	light2->setModel(sphere2);
	Renderer::getInstance().addLights(light2);

	Material* material3 = new Material();
	material3->setAmbient(glm::vec3(0.3, 0.3, 0.3));
	material3->setDiffuse(glm::vec3(1.0, 1.0, 1.0));
	material3->setSpecular(glm::vec3(0.5, 0.5, 0.5));
	material3->setShininess(2.0f);
	material3->setReflection(1.0f);

	Model cube;
	glm::mat4 cubeMat = glm::translate(glm::mat4(1.0), glm::vec3(-1.0, 2.0, 1.0));
	cube.setMatrix(cubeMat);
	cubeMesh->setTexture(texture3);
	cube.setMesh(cubeMesh);
	cube.setMaterial(material3);
	cube.setProgram(program);
	cube.name = "cube";
	models.push_back(cube);


	Material* material4 = new Material();
	material4->setAmbient(glm::vec3(0.3, 0.3, 0.3));
	material4->setDiffuse(glm::vec3(1.0, 1.0, 1.0));
	material4->setSpecular(glm::vec3(0.5, 0.5, 0.5));
	material4->setShininess(2.0f);
	material4->setRefraction(6.0f);
	material4->setRefractionCoefficient(0.5f);
	Model monkey;
	glm::mat4 monkeyMat = glm::translate(glm::mat4(1.0), glm::vec3(3.0, 2.0, 1.0));
	monkeyMat = glm::scale(monkeyMat, glm::vec3(0.5, 0.5, 0.5));
	monkey.setMatrix(monkeyMat);
	monkeyMesh->setTexture(texture);
	monkey.setMesh(monkeyMesh);
	monkey.setMaterial(material4);
	monkey.setProgram(program);
	monkey.name = "monkey";
	models.push_back(monkey);

	mesh->setTexture(texture);
	// Potok
	glAttachShader(program->getId(), LoadShader(GL_VERTEX_SHADER, "shaders/vertex.glsl"));
	glAttachShader(program->getId(), LoadShader(GL_FRAGMENT_SHADER, "shaders/fragment.glsl"));
	LinkAndValidateProgram(program->getId());
	glAttachShader(program2->getId(), LoadShader(GL_VERTEX_SHADER, "shaders/vertexLight.glsl"));
	glAttachShader(program2->getId(), LoadShader(GL_FRAGMENT_SHADER, "shaders/fragmentLight.glsl"));
	LinkAndValidateProgram(program2->getId());
	glAttachShader(programMuliMesh->getId(), LoadShader(GL_VERTEX_SHADER, "shaders/vertex-multimesh.glsl"));
	glAttachShader(programMuliMesh->getId(), LoadShader(GL_FRAGMENT_SHADER, "shaders/fragment-multimesh.glsl"));
	LinkAndValidateProgram(programMuliMesh->getId());
	assetsRepository->registerProgram("skybox");
	Program* program3 = assetsRepository->getProgram("skybox");
	glAttachShader(program3->getId(), LoadShader(GL_VERTEX_SHADER, "shaders/skybox-vertex.glsl"));
	glAttachShader(program3->getId(), LoadShader(GL_FRAGMENT_SHADER, "shaders/skybox-fragment.glsl"));
	LinkAndValidateProgram(program3->getId());
	SkyBox* skyBox0 = new SkyBox();
	skyBox0->setRightTexture("textures/skybox/xpos.png");
	skyBox0->setLeftTexture("textures/skybox/xneg.png");
	skyBox0->setTopTexture("textures/skybox/ypos.png");
	skyBox0->setBottomTexture("textures/skybox/yneg.png");
	skyBox0->setFrontTexture("textures/skybox/zpos.png");
	skyBox0->setBackTexture("textures/skybox/zneg.png");
	skyBox0->setProgram(program3);
	skyBox0->init();
	Renderer::getInstance().addSkyBox(skyBox0);
	SkyBox* skyBox1 = new SkyBox();
	skyBox1 = new SkyBox();
	skyBox1->setRightTexture("textures/skybox1/posx.jpg");
	skyBox1->setLeftTexture("textures/skybox1/negx.jpg");
	skyBox1->setTopTexture("textures/skybox1/posy.jpg");
	skyBox1->setBottomTexture("textures/skybox1/negy.jpg");
	skyBox1->setFrontTexture("textures/skybox1/posz.jpg");
	skyBox1->setBackTexture("textures/skybox1/negz.jpg");
	skyBox1->setProgram(program3);
	skyBox1->init();
	Renderer::getInstance().addSkyBox(skyBox1);
	SkyBox* skyBox2 = new SkyBox();
	skyBox2 = new SkyBox();
	skyBox2->setRightTexture("textures/skybox2/posx.jpg");
	skyBox2->setLeftTexture("textures/skybox2/negx.jpg");
	skyBox2->setTopTexture("textures/skybox2/posy.jpg");
	skyBox2->setBottomTexture("textures/skybox2/negy.jpg");
	skyBox2->setFrontTexture("textures/skybox2/posz.jpg");
	skyBox2->setBackTexture("textures/skybox2/negz.jpg");
	skyBox2->setProgram(program3);
	skyBox2->init();
	Renderer::getInstance().addSkyBox(skyBox2);
	SkyBox* skyBox3 = new SkyBox();
	skyBox3 = new SkyBox();
	skyBox3->setRightTexture("textures/skybox3/posx.jpg");
	skyBox3->setLeftTexture("textures/skybox3/negx.jpg");
	skyBox3->setTopTexture("textures/skybox3/posy.jpg");
	skyBox3->setBottomTexture("textures/skybox3/negy.jpg");
	skyBox3->setFrontTexture("textures/skybox3/posz.jpg");
	skyBox3->setBackTexture("textures/skybox3/negz.jpg");
	skyBox3->setProgram(program3);
	skyBox3->init();
	Renderer::getInstance().addSkyBox(skyBox3);
	SkyBox* skyBox4 = new SkyBox();
	skyBox4 = new SkyBox();
	skyBox4->setRightTexture("textures/skybox4/posx.jpg");
	skyBox4->setLeftTexture("textures/skybox4/negx.jpg");
	skyBox4->setTopTexture("textures/skybox4/posy.jpg");
	skyBox4->setBottomTexture("textures/skybox4/negy.jpg");
	skyBox4->setFrontTexture("textures/skybox4/posz.jpg");
	skyBox4->setBackTexture("textures/skybox4/negz.jpg");
	skyBox4->setProgram(program3);
	skyBox4->init();
	Renderer::getInstance().addSkyBox(skyBox4);
	SkyBox* skyBox5 = new SkyBox();
	skyBox5 = new SkyBox();
	skyBox5->setRightTexture("textures/skybox5/posx.jpg");
	skyBox5->setLeftTexture("textures/skybox5/negx.jpg");
	skyBox5->setTopTexture("textures/skybox5/posy.jpg");
	skyBox5->setBottomTexture("textures/skybox5/negy.jpg");
	skyBox5->setFrontTexture("textures/skybox5/posz.jpg");
	skyBox5->setBackTexture("textures/skybox5/negz.jpg");
	skyBox5->setProgram(program3);
	skyBox5->init();
	Renderer::getInstance().addSkyBox(skyBox5);

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
	Renderer::getInstance().increaseTime(0.01f);
	glutPostRedisplay();
	for (Light* light : Renderer::getInstance().getLights()) {
		if (Settings::getInstance().animateLight) {
			light->setPosition(glm::vec3(2.0 * sin(Renderer::getInstance().getTime()), 3.2, 1.0 * cos(Renderer::getInstance().getTime())));
		}
		else {
			light->setPosition(glm::vec3(2.0, 3.2, 1.0));
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
	matProj = glm::perspective(glm::radians(90.0f), width / (float)height, 0.01f, 100.0f);
	float aspect = width / (float)height;
	float zoom = 5.0;
	minimapProj = glm::ortho(-aspect * zoom, aspect * zoom, -aspect * zoom, aspect * zoom, 0.01f, 100.0f);
	lightProj = glm::ortho(-40.0f, 40.0f, -40.0f, 40.0f, -40.0f, 40.0f);

	Renderer::getInstance().setLightProj(lightProj);

	// Aktualizowanie rozmiaru tekstury
	glBindTexture(GL_TEXTURE_2D, minimapDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, minimapTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindRenderbuffer(GL_RENDERBUFFER, postprocessRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	glBindTexture(GL_TEXTURE_2D, postprocessDepth);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT32F, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_BYTE, NULL);
	glBindTexture(GL_TEXTURE_2D, postprocessTexture);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA32F, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0, GL_RGBA, GL_UNSIGNED_BYTE, NULL);
	glBindRenderbuffer(GL_RENDERBUFFER, postprocessRBO);
	glRenderbufferStorage(GL_RENDERBUFFER, GL_DEPTH24_STENCIL8, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));

	// Aktualizowanie rozmiarów i położenia spriteów
	for (Sprite sprite : sprites) {
		if(sprite.getName() == "postprocess")
			sprite.setSize(glm::vec2(glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT)));
		sprite.updateVertices();
	}
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

	case 101:
		Settings::getInstance().fogVisibility = false;
		break;
	case 102:
		Settings::getInstance().fogVisibility = true;
		break;

	case 200:
		Settings::getInstance().skyBox = 0;
		break;
	case 201:
		Settings::getInstance().skyBox = 1;
		break;
	case 202:
		Settings::getInstance().skyBox = 2;
		break;
	case 203:
		Settings::getInstance().skyBox = 3;
		break;
	case 204:
		Settings::getInstance().skyBox = 4;
		break;
	case 205:
		Settings::getInstance().skyBox = 5;
		break;

	case 301:
		Settings::getInstance().showRefraction = false;
		break;
	case 302:
		Settings::getInstance().showRefraction = true;
		break;
	case 311:
		Settings::getInstance().showReflection = false;
		break;
	case 312:
		Settings::getInstance().showReflection = true;
		break;

	case 401:
		Settings::getInstance().showMinimap = false;
		break;
	case 402:
		Settings::getInstance().showMinimap = true;
		break;

	case 501:
		Settings::getInstance().postProcessMode = 0;
		break;
	case 502:
		Settings::getInstance().postProcessMode = 1;
		break;
	case 503:
		Settings::getInstance().postProcessMode = 2;
		break;
	case 504:
		Settings::getInstance().postProcessMode = 3;
		break;
	case 505:
		Settings::getInstance().postProcessMode = 4;
		break;
	case 506:
		Settings::getInstance().postProcessMode = 5;
		break;
	case 507:
		Settings::getInstance().postProcessMode = 6;
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

	int fogVisibilityMenu = glutCreateMenu(processMenu);
	glutAddMenuEntry("Disable", 101);
	glutAddMenuEntry("Enable", 102);

	int fog = glutCreateMenu(processMenu);
	glutAddSubMenu("Visibility", fogVisibilityMenu);

	int skyBox = glutCreateMenu(processMenu);
	glutAddMenuEntry("0", 200);
	glutAddMenuEntry("1", 201);
	glutAddMenuEntry("2", 202);
	glutAddMenuEntry("3", 203);
	glutAddMenuEntry("4", 204);
	glutAddMenuEntry("5", 205);

	int materialsRefraction = glutCreateMenu(processMenu);
	glutAddMenuEntry("Disable", 301);
	glutAddMenuEntry("Enable", 302);

	int materialsReflection = glutCreateMenu(processMenu);
	glutAddMenuEntry("Disable", 311);
	glutAddMenuEntry("Enable", 312);

	int materials = glutCreateMenu(processMenu);
	glutAddSubMenu("Refraction", materialsRefraction);
	glutAddSubMenu("Reflection", materialsReflection);

	int minimapVisibilityMenu = glutCreateMenu(processMenu);
	glutAddMenuEntry("Disable", 401);
	glutAddMenuEntry("Enable", 402);

	int minimap = glutCreateMenu(processMenu);
	glutAddSubMenu("Visibility", minimapVisibilityMenu);

	int postprocess = glutCreateMenu(processMenu);
	glutAddMenuEntry("Standard", 501);
	glutAddMenuEntry("Grayscale", 502);
	glutAddMenuEntry("Negative", 503);
	glutAddMenuEntry("Sharpening", 504);
	glutAddMenuEntry("Blur", 505);
	glutAddMenuEntry("Edge", 506);
	glutAddMenuEntry("Vignette ", 507);

	int menu = glutCreateMenu(processMenu);
	glutAddSubMenu("Light", light);
	glutAddSubMenu("Fog", fog);
	glutAddSubMenu("SkyBox", skyBox);
	glutAddSubMenu("Materials", materials);
	glutAddSubMenu("Minimapa", minimap);
	glutAddSubMenu("Postprocessy", postprocess);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

// ---------------------------------------------------
int main(int argc, char* argv[])
{
	// GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Programownie grafiki 3D w OpenGL");

	__CHECK_FOR_ERRORS;
	glutDisplayFunc(Render);
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
