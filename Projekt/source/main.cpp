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
#include "PerlinNoise.hpp"
#include "text-ft.hpp"

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
#include "Terrain.h"

// Macierze PVM
glm::mat4 matProj;
glm::mat4 minimapProj;
glm::mat4 lightProj;
glm::mat4 matView = glm::mat4(1.0);

std::vector<Sprite> sprites;
std::vector<Model> models;

Sprite* rockIcon;
Sprite* woodIcon;


constexpr float one_degree = 3.14 / 180.f;

Material* testMultiMeshMaterial = new Material();
glm::mat4 multiMeshMatrix = glm::mat4(1.0);
glm::vec3 multiMeshColor = glm::vec3(1.0);
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

std::vector<glm::vec2> treeMultiMeshPositions;
std::vector<glm::vec2> rockMultiMeshPositions;

Texture* postProcessTexture = new Texture();
Texture* miniMapTexture = new Texture();

int woodCount = 1;
int rockCount = 2;

int castleCount = 0;
int sawmillCount = 0;
int mineCount = 0;
int houseCount = 0;

int selected = 0;
std::string selectedName = "castle";

unsigned char* newData = new unsigned char[256 * 256 * 4];
unsigned int main_seed = 123456u;
std::vector<Model> buildings;
int needWood(std::string name) {
	if (name == "sawmill") {
		return std::pow(2, sawmillCount) / 2;
	}
	if (name == "mine") {
		return mineCount;
	}
	if (name == "house") {
		return std::pow(2, houseCount);
	}
	else {
		return 0;
	}
}
int needRock(std::string name) {
	if (name == "sawmill") {
		return sawmillCount * 2;
	}
	if (name == "mine") {
		return std::pow(2, houseCount) / 2;
	}
	if (name == "house") {
		return std::pow(2, houseCount) / 2;
	}
	else {
		return 0;
	}
}
bool buildingDistance(float mx, float my) {
	float minDistance = 1000.0;
	if (buildings.empty()) return true;
	for (Model model : buildings) {
		float distance = glm::distance(glm::vec2(mx, my), model.getMesh()->getPosition());
		if (distance < minDistance) minDistance = distance;
	}
	return minDistance > 2.0 && minDistance < 4.0;
}
bool treeDistance(float mx, float my) {
	float minDistance = 1000.0;
	for (glm::vec2 pos : treeMultiMeshPositions) {
		float distance = glm::distance(glm::vec2(mx, my), pos);
		if (distance < minDistance) minDistance = distance;
	}
	if (selected == 1 && minDistance > 3.0) return false;
	return minDistance > 0.75;
}
bool rockDistance(float mx, float my) {
	float minDistance = 1000.0;
	for (glm::vec2 pos : rockMultiMeshPositions) {
		float distance = glm::distance(glm::vec2(mx, my), pos);
		if (distance < minDistance) minDistance = distance;
	}
	if (selected == 2 && minDistance > 3.0) return false;
	return minDistance > 0.75;
}
int validSpot(glm::vec2 pos) {
	// water distance
	int x = ((int)pos.x + 64.0f) * (256.0f / 128.0f);
	int y = ((int)pos.y + 64.0f) * (256.0f / 128.0f);
	bool newState = (int)newData[(x + (256 - y) * 256) * 4] > ((selected == 0) ? 110 : 106);
	//castle
	if (selected == 0 && castleCount > 0) newState = false;
	if (selected != 0 && castleCount == 0) newState = false;
	//rock needed
	if(needRock(selectedName) > rockCount) newState = false;
	//wood needed
	if (needWood(selectedName) > woodCount) newState = false;
	//space allowed
		// other building
		if(!buildingDistance(pos.x, pos.y)) newState = false;
		// enviroment
		if (!treeDistance(pos.x, pos.y)) newState = false;
		if (!rockDistance(pos.x, pos.y)) newState = false;
		
	return (newState) ? 1 : 2;
}

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
	CameraMatProj = matProj;
	// -----------------------------------------------
	// NOWE : wyliczanie pozycji kamery z matView
	// -----------------------------------------------
	glm::vec3 cameraPos = ExtractCameraPos(matView);
	Renderer::getInstance().getSkyBoxes().at(Settings::getInstance().skyBox)->setPosition(cameraPos);
	Renderer::getInstance().getSkyBoxes().at(Settings::getInstance().skyBox)->draw(matProj, matView);
	Renderer::getInstance().getTerrain()->draw(cameraPos, matProj, matView); // terrain
	Renderer::getInstance().getRocks()->draw(assetsRepository->getProgram("multimesh"), multiMeshMatrix, cameraPos, matProj, matView, testMultiMeshMaterial, multiMeshColor); // rock
	Renderer::getInstance().getTrees()->draw(assetsRepository->getProgram("multimesh"), multiMeshMatrix, cameraPos, matProj, matView, testMultiMeshMaterial, multiMeshColor); // trees
	Renderer::getInstance().getWater()->draw(cameraPos, matProj, matView); // water
	glReadPixels(_mouse_X, glutGet(GLUT_WINDOW_HEIGHT) - _mouse_Y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

	int idx = 0;
	for (Model model : models) {
		if (idx != selected) {
			idx++;
			continue;
		}
		model.getMesh()->setState(validSpot(glm::vec2(worldPoint.x, worldPoint.z)));
		model.setMatrix(glm::translate(model.getMatrix(), worldPoint));
		model.setMatrix(glm::scale(model.getMatrix(), model.getScale()));
		model.setPosition(glm::vec2(worldPoint.x, worldPoint.z));
		model.draw(cameraPos, matProj, matView);
		idx++;
	}
	for (Model model : buildings) {
		model.getMesh()->setState(0);
		model.setMatrix(glm::scale(model.getMatrix(), model.getScale()));
		model.draw(cameraPos, matProj, matView);
	}
}
void Minimap()
{
	glViewport(0, 0, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT));
	//glViewport(0, 0, 1024, 1024);
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
	Renderer::getInstance().getTerrain()->draw(cameraPos, minimapProj, matView);
	for (Model model : buildings) {
		model.getMesh()->setState(0);
		model.setMatrix(glm::scale(model.getMatrix(), model.getScale()));
		model.draw(cameraPos, minimapProj, matView);
	}
	Renderer::getInstance().getRocks()->draw(assetsRepository->getProgram("multimesh"), multiMeshMatrix, cameraPos, minimapProj, matView, testMultiMeshMaterial, multiMeshColor); // rocks
	Renderer::getInstance().getTrees()->draw(assetsRepository->getProgram("multimesh"), multiMeshMatrix, cameraPos, minimapProj, matView, testMultiMeshMaterial, multiMeshColor); // trees
	Renderer::getInstance().getWater()->draw(cameraPos, minimapProj, matView);
	Settings::getInstance().lightVisibility = lightState;
}

void Shadows()
{
	glViewport(0, 0, 16384, 16384);
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
	glm::vec3 lightPosition = glm::vec3(cameraPos.x+2.0, 3.0, cameraPos.z+1.0);
	glm::vec3 Light_Direction = glm::vec3(cameraPos.x, 0.0, cameraPos.z);
	glm::mat4 lightView = glm::lookAt(lightPosition, Light_Direction, glm::vec3(0.0f, 1.0f, 0.0f));
	Renderer::getInstance().setLightView(lightView);
	Renderer::getInstance().getTerrain()->draw(lightPosition, lightProj, lightView);

	int idx = 0;
	for (Model model : models) {
		if (idx != selected) {
			idx++;
			continue; 
		}
		model.setMatrix(glm::translate(model.getMatrix(), worldPoint));
		model.setMatrix(glm::scale(model.getMatrix(), model.getScale()));
		model.setPosition(glm::vec2(worldPoint.x, worldPoint.z));
		model.draw(lightPosition, lightProj, lightView);
		selectedName = model.name;
		idx++;
	}
	for (Model model : buildings) {
		model.setMatrix(glm::scale(model.getMatrix(), model.getScale()));
		model.draw(lightPosition, lightProj, lightView);
	}
	Renderer::getInstance().getRocks()->draw(assetsRepository->getProgram("multimesh"), multiMeshMatrix, lightPosition, lightProj, lightView, testMultiMeshMaterial, multiMeshColor); // rocks
	Renderer::getInstance().getTrees()->draw(assetsRepository->getProgram("multimesh"), multiMeshMatrix, lightPosition, lightProj, lightView, testMultiMeshMaterial, multiMeshColor); // tree
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
	//glReadPixels(_mouse_X, glutGet(GLUT_WINDOW_HEIGHT) - _mouse_Y, 1, 1, GL_DEPTH_COMPONENT, GL_FLOAT, &winZ);

	glBindFramebuffer(GL_FRAMEBUFFER, 0);

	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT | GL_STENCIL_BUFFER_BIT);
	glClearColor(0.3f, 0.3f, 0.3f, 1.0f);
	glDisable(GL_DEPTH_TEST);

	for (Sprite sprite : sprites) {
		if (sprite.getName() == "minimap" && Settings::getInstance().showMinimap) {
			sprite.draw();
		}
		else if (sprite.getName() == "castle"){
			sprite.setPosition(glm::vec2(glutGet(GLUT_WINDOW_WIDTH)/2.0 - 275.0, glutGet(GLUT_WINDOW_HEIGHT) - 200.0));
			sprite.draw();

			rockIcon->setPosition(glm::vec2(glutGet(GLUT_WINDOW_WIDTH) / 2.0 - 275.0, glutGet(GLUT_WINDOW_HEIGHT) - 100.0));
			rockIcon->setSize(glm::vec2(50.0));
			rockIcon->draw();

			woodIcon->setPosition(glm::vec2(glutGet(GLUT_WINDOW_WIDTH) / 2.0 - 275.0, glutGet(GLUT_WINDOW_HEIGHT) - 50.0));
			woodIcon->setSize(glm::vec2(50.0));
			woodIcon->draw();
		}
		else if (sprite.getName() == "sawmill") {
			sprite.setPosition(glm::vec2(glutGet(GLUT_WINDOW_WIDTH) / 2.0 - 125.0, glutGet(GLUT_WINDOW_HEIGHT) - 200.0));
			sprite.draw();

			rockIcon->setPosition(glm::vec2(glutGet(GLUT_WINDOW_WIDTH) / 2.0 - 125.0, glutGet(GLUT_WINDOW_HEIGHT) - 100.0));
			rockIcon->setSize(glm::vec2(50.0));
			rockIcon->draw();

			woodIcon->setPosition(glm::vec2(glutGet(GLUT_WINDOW_WIDTH) / 2.0 - 125.0, glutGet(GLUT_WINDOW_HEIGHT) - 50.0));
			woodIcon->setSize(glm::vec2(50.0));
			woodIcon->draw();
		}
		else if (sprite.getName() == "mine") {
			sprite.setPosition(glm::vec2(glutGet(GLUT_WINDOW_WIDTH) / 2.0 + 25.0, glutGet(GLUT_WINDOW_HEIGHT) - 200.0));
			sprite.draw();

			rockIcon->setPosition(glm::vec2(glutGet(GLUT_WINDOW_WIDTH) / 2.0 + 25.0, glutGet(GLUT_WINDOW_HEIGHT) - 100.0));
			rockIcon->setSize(glm::vec2(50.0));
			rockIcon->draw();

			woodIcon->setPosition(glm::vec2(glutGet(GLUT_WINDOW_WIDTH) / 2.0 + 25.0, glutGet(GLUT_WINDOW_HEIGHT) - 50.0));
			woodIcon->setSize(glm::vec2(50.0));
			woodIcon->draw();
		}
		else if (sprite.getName() == "house") {
			sprite.setPosition(glm::vec2(glutGet(GLUT_WINDOW_WIDTH) / 2.0 + 175.0, glutGet(GLUT_WINDOW_HEIGHT) - 200.0));
			sprite.draw();

			rockIcon->setPosition(glm::vec2(glutGet(GLUT_WINDOW_WIDTH) / 2.0 + 175.0, glutGet(GLUT_WINDOW_HEIGHT) - 100.0));
			rockIcon->setSize(glm::vec2(50.0));
			rockIcon->draw();

			woodIcon->setPosition(glm::vec2(glutGet(GLUT_WINDOW_WIDTH) / 2.0 + 175.0, glutGet(GLUT_WINDOW_HEIGHT) - 50.0));
			woodIcon->setSize(glm::vec2(50.0));
			woodIcon->draw();
		}
		else {
			sprite.draw();
		}
	}
	// resource info
	woodIcon->setPosition(glm::vec2(75.0, 250.0));
	woodIcon->setSize(glm::vec2(50.0));
	woodIcon->draw();
	rockIcon->setPosition(glm::vec2(75.0, 300.0));
	rockIcon->setSize(glm::vec2(50.0));
	rockIcon->draw();

	//char txt[255];
	//sprintf_s(txt, sizeof(txt), "Frame: %d", 0);

	//RenderText(txt, 25, 25, 1.0f, glm::vec3(0.0));
	//RenderText("ESC - Exit", 25, 450, 0.5f, glm::vec3(0.3, 0.7f, 0.9f));

	//RenderText("Napis", 25, 70, 0.5f, glm::vec3(0.1, 0.1, 1.0f));

	char txt[255];
	//resource
	sprintf_s(txt, sizeof(txt), "%d", woodCount);
	RenderText(txt, 125.0, glutGet(GLUT_WINDOW_HEIGHT) - 290.0, 1.0f, glm::vec3(1.0));
	sprintf_s(txt, sizeof(txt), "%d", rockCount);
	RenderText(txt, 125.0, glutGet(GLUT_WINDOW_HEIGHT) - 340.0, 1.0f, glm::vec3(1.0));

	//castle
	//int needRock = 10;
	//int needWood = 0;
	RenderText("1", glutGet(GLUT_WINDOW_WIDTH) / 2.0 - 235.0, 210.0, 1.0f, (selected == 0) ? glm::vec3(1.0, 1.0, 0.0) : glm::vec3(1.0));
	sprintf_s(txt, sizeof(txt), "%d", needRock("castle"));
	RenderText(txt, glutGet(GLUT_WINDOW_WIDTH) / 2.0 - 225.0, 60.0, 1.0f, (rockCount < needRock("castle")) ? glm::vec3(1.0, 0.0, 0.0) : glm::vec3(0.0, 1.0, 0.0));
	sprintf_s(txt, sizeof(txt), "%d", needWood("castle"));
	RenderText(txt, glutGet(GLUT_WINDOW_WIDTH) / 2.0 - 225.0, 10.0, 1.0f, (woodCount < needWood("castle")) ? glm::vec3(1.0, 0.0, 0.0) : glm::vec3(0.0, 1.0, 0.0));

	//sawmill
	RenderText("2", glutGet(GLUT_WINDOW_WIDTH) / 2.0 - 85.0, 210.0, 1.0f, (selected == 1) ? glm::vec3(1.0, 1.0, 0.0) : glm::vec3(1.0));
	sprintf_s(txt, sizeof(txt), "%d", needRock("sawmill"));
	RenderText(txt, glutGet(GLUT_WINDOW_WIDTH) / 2.0 - 75.0, 60.0, 1.0f, (rockCount < needRock("sawmill")) ? glm::vec3(1.0, 0.0, 0.0) : glm::vec3(0.0, 1.0, 0.0));
	sprintf_s(txt, sizeof(txt), "%d", needWood("sawmill"));
	RenderText(txt, glutGet(GLUT_WINDOW_WIDTH) / 2.0 - 75.0, 10.0, 1.0f, (woodCount < needWood("sawmill")) ? glm::vec3(1.0, 0.0, 0.0) : glm::vec3(0.0, 1.0, 0.0));

	//mine
	RenderText("3", glutGet(GLUT_WINDOW_WIDTH) / 2.0 + 65.0, 210.0, 1.0f, (selected == 2) ? glm::vec3(1.0, 1.0, 0.0) : glm::vec3(1.0));
	sprintf_s(txt, sizeof(txt), "%d", needRock("mine"));
	RenderText(txt, glutGet(GLUT_WINDOW_WIDTH) / 2.0 + 75.0, 60.0, 1.0f, (rockCount < needRock("mine")) ? glm::vec3(1.0, 0.0, 0.0) : glm::vec3(0.0, 1.0, 0.0));
	sprintf_s(txt, sizeof(txt), "%d", needWood("mine"));
	RenderText(txt, glutGet(GLUT_WINDOW_WIDTH) / 2.0 + 75.0, 10.0, 1.0f, (woodCount < needWood("mine")) ? glm::vec3(1.0, 0.0, 0.0) : glm::vec3(0.0, 1.0, 0.0));

	//house
	RenderText("4", glutGet(GLUT_WINDOW_WIDTH) / 2.0 + 215.0, 210.0, 1.0f, (selected == 3) ? glm::vec3(1.0, 1.0, 0.0) : glm::vec3(1.0));
	sprintf_s(txt, sizeof(txt), "%d", needRock("house"));
	RenderText(txt, glutGet(GLUT_WINDOW_WIDTH) / 2.0 + 225.0, 60.0, 1.0f, (rockCount < needRock("house")) ? glm::vec3(1.0, 0.0, 0.0) : glm::vec3(0.0, 1.0, 0.0));
	sprintf_s(txt, sizeof(txt), "%d", needWood("house"));
	RenderText(txt, glutGet(GLUT_WINDOW_WIDTH) / 2.0 + 225.0, 10.0, 1.0f, (woodCount < needWood("house")) ? glm::vec3(1.0, 0.0, 0.0) : glm::vec3(0.0, 1.0, 0.0));

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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, glutGet(GLUT_WINDOW_WIDTH), glutGet(GLUT_WINDOW_HEIGHT), 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
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
	glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT, 16384, 16384, 0, GL_DEPTH_COMPONENT, GL_FLOAT, NULL);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);
	float borderColor[] = { 1.0f, 1.0f, 1.0f, 1.0f };
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
	Terrain* terrain = new Terrain();
	Renderer::getInstance().setTerrain(terrain);
	Water* water = new Water();
	Renderer::getInstance().setWater(water);
	assetsRepository->registerTexture("grass", "textures/grass.png", GL_RGBA);
	Texture* grass = assetsRepository->getTexture("grass");
	assetsRepository->registerTexture("dirt", "textures/dirt.png", GL_RGBA);
	Texture* dirt = assetsRepository->getTexture("dirt");
	assetsRepository->registerTexture("sand", "textures/sand.png", GL_RGBA);
	Texture* sand = assetsRepository->getTexture("sand");
	assetsRepository->registerTexture("water", "textures/water.jpg", GL_RGB);
	Texture* waterTexture = assetsRepository->getTexture("water");
	terrain->setTexture1(grass);
	terrain->setTexture2(dirt);
	terrain->setTexture3(sand);
	water->setTexture(waterTexture);

	InitText("fonts/arial.ttf", 36);
	// ------------------------------------------------------------
	// Ustawienie domyslnego odsuniecia kamery od polozenia (0,0,0)
	// ------------------------------------------------------------
	CameraTranslate_x = 0.0;
	CameraTranslate_y = -1.0;
	CameraTranslate_z = -12.0;

	multiMeshMatrix = glm::scale(multiMeshMatrix, glm::vec3(4.0));
	testMultiMeshMaterial->setAmbient(glm::vec3(0.1, 0.1, 0.1));
	testMultiMeshMaterial->setDiffuse(glm::vec3(0.2, 0.2, 0.2));
	testMultiMeshMaterial->setSpecular(glm::vec3(0.1, 0.1, 0.1));
	testMultiMeshMaterial->setShininess(1.0f);
	// Wczytanie pliku obj
	terrain->load("models/terrain256.obj");
	water->load("models/terrain256.obj");
	Material* terrainMaterial = new Material();
	terrainMaterial->setAmbient(glm::vec3(0.1, 0.1, 0.1));
	terrainMaterial->setDiffuse(glm::vec3(0.1, 0.1, 0.1));
	terrainMaterial->setSpecular(glm::vec3(0.1, 0.1, 0.1));
	terrainMaterial->setShininess(1.0f);
	terrain->setMaterial(terrainMaterial);
	Material* waterMaterial = new Material();
	waterMaterial->setAmbient(glm::vec3(0.3, 0.3, 0.3));
	waterMaterial->setDiffuse(glm::vec3(0.5, 0.5, 0.5));
	waterMaterial->setSpecular(glm::vec3(0.5, 0.5, 0.5));
	waterMaterial->setShininess(2.0f);
	waterMaterial->setReflection(8.0f);
	water->setMaterial(waterMaterial);


	// Heightmapa
	const siv::PerlinNoise::seed_type seed = main_seed;
	const siv::PerlinNoise perlin{ seed };
	for (int y = 0; y < 256; ++y) {
		for (int x = 0; x < 256; ++x) {
			const double noise = perlin.octave2D_01((x * 0.01), (y * 0.01), 4);
			newData[y * 256 * 4 + x * 4] = static_cast<unsigned char>(std::max(0, std::min(255, static_cast<int>(floor(noise * 256.0)))));
			newData[y * 256 * 4 + x * 4 + 1] = 0;
			newData[y * 256 * 4 + x * 4 + 2] = 0;
			newData[y * 256 * 4 + x * 4 + 3] = 0;
		}
	}
	Texture* heightmap = new Texture();
	heightmap->raw(newData, 256, 256, 4, GL_RGBA);
	terrain->setHeightmap(heightmap);
	water->setHeightmap(heightmap);


	MultiMesh* treeMultiMesh = new MultiMesh();
	treeMultiMesh->load("models/tree.obj");
	assetsRepository->registerTexture("tree", "textures/tree.png", GL_RGBA);
	Texture* tree = assetsRepository->getTexture("tree");
	treeMultiMesh->setTexture(tree);
	treeMultiMesh->setHeightmap(heightmap);

	MultiMesh* rockMultiMesh = new MultiMesh();
	rockMultiMesh->load("models/rock.obj");
	assetsRepository->registerTexture("rock", "textures/rock.png", GL_RGB);
	Texture* rock = assetsRepository->getTexture("rock");
	rockMultiMesh->setTexture(rock);
	rockMultiMesh->setHeightmap(heightmap);

	std::vector<glm::mat4x4> treeMultiMeshMatrixes;
	std::srand(static_cast<unsigned int>(time(NULL)));
	std::mt19937 gen(seed);
	for (int i = 0; i < 2000; ++i)
	{
		glm::mat4x4 treeMultiMeshMatrix = glm::mat4x4(1.0); 
		float x, z;
		int _x, _z;
		do {
			x = std::uniform_real_distribution<float>(-63.0, 63.0)(gen);
			z = std::uniform_real_distribution<float>(-63.0, 63.0)(gen);
			_x = ((int)(x) + 64)*2;
			_z = ((int)(z) + 64)*2;
		} while ((int)newData[(_x + (256-_z) * 256) * 4] < 108);
		treeMultiMeshMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(
			x, 0.0, z
		));
		float r = std::uniform_real_distribution<float>(-6.28, 6.28)(gen);
		treeMultiMeshMatrix = glm::rotate(treeMultiMeshMatrix, r, glm::vec3(0.0f, 1.0f, 0.0f));
		treeMultiMeshMatrix = glm::scale(treeMultiMeshMatrix, glm::vec3(0.1));
		treeMultiMeshPositions.push_back(glm::vec2(x, z));
		treeMultiMeshMatrixes.push_back(treeMultiMeshMatrix);
	}
	std::vector<glm::mat4x4> rockMultiMeshMatrixes;
	for (int i = 0; i < 100; ++i)
	{
		glm::mat4x4 rockMultiMeshMatrix = glm::mat4x4(1.0);
		float x, z;
		int _x, _z;
		do {
			x = std::uniform_real_distribution<float>(-63.0, 63.0)(gen);
			z = std::uniform_real_distribution<float>(-63.0, 63.0)(gen);
			_x = ((int)(x)+64) * 2;
			_z = ((int)(z)+64) * 2;
		} while ((int)newData[(_x + (256 - _z) * 256) * 4] < 108);
		rockMultiMeshMatrix = glm::translate(glm::mat4(1.0f), glm::vec3(
			x, 0.0, z
		));
		float r = std::uniform_real_distribution<float>(-6.28, 6.28)(gen);
		rockMultiMeshMatrix = glm::rotate(rockMultiMeshMatrix, r, glm::vec3(0.0f, 1.0f, 0.0f));
		rockMultiMeshMatrix = glm::scale(rockMultiMeshMatrix, glm::vec3(0.1));
		rockMultiMeshPositions.push_back(glm::vec2(x, z));
		rockMultiMeshMatrixes.push_back(rockMultiMeshMatrix);
	}

	treeMultiMesh->setMatrixes(treeMultiMeshMatrixes);
	treeMultiMesh->setPositions(treeMultiMeshPositions);
	Renderer::getInstance().setTrees(treeMultiMesh);
	rockMultiMesh->setMatrixes(rockMultiMeshMatrixes);
	rockMultiMesh->setPositions(rockMultiMeshPositions);
	Renderer::getInstance().setRocks(rockMultiMesh);

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

	assetsRepository->registerProgram("image");
	Program* programImage = assetsRepository->getProgram("image");
	glAttachShader(programImage->getId(), LoadShader(GL_VERTEX_SHADER, "shaders/image-vert.glsl"));
	glAttachShader(programImage->getId(), LoadShader(GL_FRAGMENT_SHADER, "shaders/image-frag.glsl"));
	LinkAndValidateProgram(programImage->getId());

	assetsRepository->registerProgram("shadow");
	Program* programShadow = assetsRepository->getProgram("shadow");
	glAttachShader(programShadow->getId(), LoadShader(GL_VERTEX_SHADER, "shaders/depthmap-vertex.glsl"));
	glAttachShader(programShadow->getId(), LoadShader(GL_FRAGMENT_SHADER, "shaders/depthmap-fragment.glsl"));
	LinkAndValidateProgram(programShadow->getId());

	assetsRepository->registerProgram("terrain");
	Program* terrainProgram = assetsRepository->getProgram("terrain");
	glAttachShader(terrainProgram->getId(), LoadShader(GL_VERTEX_SHADER, "shaders/vertex-terrain.glsl"));
	glAttachShader(terrainProgram->getId(), LoadShader(GL_FRAGMENT_SHADER, "shaders/fragment-terrain.glsl"));
	LinkAndValidateProgram(terrainProgram->getId());
	terrain->setProgram(terrainProgram);

	assetsRepository->registerProgram("water");
	Program* waterProgram = assetsRepository->getProgram("water");
	glAttachShader(waterProgram->getId(), LoadShader(GL_VERTEX_SHADER, "shaders/vertex-water.glsl"));
	glAttachShader(waterProgram->getId(), LoadShader(GL_FRAGMENT_SHADER, "shaders/fragment-water.glsl"));
	LinkAndValidateProgram(waterProgram->getId());
	water->setProgram(waterProgram);

	assetsRepository->registerProgram("building");
	Program* buildingProgram = assetsRepository->getProgram("building");
	glAttachShader(buildingProgram->getId(), LoadShader(GL_VERTEX_SHADER, "shaders/vertex-building.glsl"));
	glAttachShader(buildingProgram->getId(), LoadShader(GL_FRAGMENT_SHADER, "shaders/fragment-building.glsl"));
	LinkAndValidateProgram(buildingProgram->getId());

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
	minimapSprite.setFlipX(true);
	minimapSprite.setFlipY(true);
	sprites.push_back(minimapSprite);

	assetsRepository->registerTexture("castle", "textures/castle-icon.png", GL_RGBA);
	Texture* castleTexture = assetsRepository->getTexture("castle");
	Sprite castleSprite;
	castleSprite.setTexture(castleTexture);
	castleSprite.setPosition(glm::vec2(100.0, 100.0));
	castleSprite.setSize(glm::vec2(100.0, 100.0));
	castleSprite.setProgram(assetsRepository->getProgram("image"));
	castleSprite.setName("castle");
	sprites.push_back(castleSprite);

	assetsRepository->registerTexture("sawmill", "textures/sawmill-icon.png", GL_RGBA);
	Texture* sawmillTexture = assetsRepository->getTexture("sawmill");
	Sprite sawmillSprite;
	sawmillSprite.setTexture(sawmillTexture);
	sawmillSprite.setPosition(glm::vec2(100.0, 100.0));
	sawmillSprite.setSize(glm::vec2(100.0, 100.0));
	sawmillSprite.setProgram(assetsRepository->getProgram("image"));
	sawmillSprite.setName("sawmill");
	sprites.push_back(sawmillSprite);

	assetsRepository->registerTexture("mine", "textures/mine-icon.png", GL_RGBA);
	Texture* mineTexture = assetsRepository->getTexture("mine");
	Sprite mineSprite;
	mineSprite.setTexture(mineTexture);
	mineSprite.setPosition(glm::vec2(100.0, 100.0));
	mineSprite.setSize(glm::vec2(100.0, 100.0));
	mineSprite.setProgram(assetsRepository->getProgram("image"));
	mineSprite.setName("mine");
	sprites.push_back(mineSprite);

	assetsRepository->registerTexture("house", "textures/house-icon.png", GL_RGBA);
	Texture* houseTexture = assetsRepository->getTexture("house");
	Sprite houseSprite;
	houseSprite.setTexture(houseTexture);
	houseSprite.setPosition(glm::vec2(100.0, 100.0));
	houseSprite.setSize(glm::vec2(100.0, 100.0));
	houseSprite.setProgram(assetsRepository->getProgram("image"));
	houseSprite.setName("house");
	sprites.push_back(houseSprite);

	assetsRepository->registerTexture("rockRes", "textures/rock-icon.png", GL_RGBA);
	Texture* rockResTexture = assetsRepository->getTexture("rockRes");
	Sprite* rockRes = new Sprite();
	rockRes->setTexture(rockResTexture);
	rockRes->setPosition(glm::vec2(100.0, 100.0));
	rockRes->setSize(glm::vec2(100.0, 100.0));
	rockRes->setProgram(assetsRepository->getProgram("image"));
	rockRes->setName("rockRes");
	rockIcon = rockRes;

	assetsRepository->registerTexture("woodRes", "textures/wood-icon.png", GL_RGBA);
	Texture* woodResTexture = assetsRepository->getTexture("woodRes");
	Sprite* woodRes = new Sprite();
	woodRes->setTexture(woodResTexture);
	woodRes->setPosition(glm::vec2(100.0, 100.0));
	woodRes->setSize(glm::vec2(100.0, 100.0));
	woodRes->setProgram(assetsRepository->getProgram("image"));
	woodRes->setName("woodRes");
	woodIcon = woodRes;

	Light* light = new Light();
	light->setAmbient(glm::vec3(0.1, 0.1, 0.1));
	light->setDiffuse(glm::vec3(0.3, 0.3, 0.3));
	light->setSpecular(glm::vec3(0.5, 0.5, 0.5));
	light->setAttenuation(glm::vec3(1.0, 1.0, 1.0));
	light->setPosition(glm::vec3(2.0, 3.0, 1.0));
	light->setIsDirectional(true);
	Renderer::getInstance().addLights(light);

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

	assetsRepository->registerTexture("castleBuilding", "textures/castle.png", GL_RGB);
	Texture* castleBuilding = assetsRepository->getTexture("castleBuilding");
	Material* castleMaterial = new Material();
	castleMaterial->setAmbient(glm::vec3(0.2, 0.2, 0.2));
	castleMaterial->setDiffuse(glm::vec3(1.0, 1.0, 1.0));
	castleMaterial->setSpecular(glm::vec3(2.0, 2.0, 2.0));
	castleMaterial->setShininess(32.0f);
	Model castle;
	castle.setMatrix(glm::mat4(1.0));
	castle.setScale(glm::vec3(1.0));
	assetsRepository->registerMesh("castle", "models/castle.obj");
	Mesh* castleMesh = assetsRepository->getMesh("castle");
	castleMesh->setTexture(castleBuilding);
	castleMesh->setHeightmap(heightmap);
	castleMesh->setPosition(glm::vec2(0.0));
	castle.setMesh(castleMesh);
	castle.setMaterial(castleMaterial);
	castle.setProgram(buildingProgram);
	castle.name = "castle";
	models.push_back(castle);

	assetsRepository->registerTexture("sawmillBuilding", "textures/sawmill.png", GL_RGB);
	Texture* sawmillBuilding = assetsRepository->getTexture("sawmillBuilding");
	Material* sawmillMaterial = new Material();
	sawmillMaterial->setAmbient(glm::vec3(0.2, 0.2, 0.2));
	sawmillMaterial->setDiffuse(glm::vec3(1.0, 1.0, 1.0));
	sawmillMaterial->setSpecular(glm::vec3(2.0, 2.0, 2.0));
	sawmillMaterial->setShininess(32.0f);
	Model sawmill;
	sawmill.setMatrix(glm::mat4(1.0));
	sawmill.setScale(glm::vec3(0.3));
	assetsRepository->registerMesh("sawmill", "models/sawmill.obj");
	Mesh* sawmillMesh = assetsRepository->getMesh("sawmill");
	sawmillMesh->setTexture(sawmillBuilding);
	sawmillMesh->setHeightmap(heightmap);
	sawmillMesh->setPosition(glm::vec2(0.0));
	sawmill.setMesh(sawmillMesh);
	sawmill.setMaterial(sawmillMaterial);
	sawmill.setProgram(buildingProgram);
	sawmill.name = "sawmill";
	models.push_back(sawmill);

	assetsRepository->registerTexture("mineBuilding", "textures/mine.png", GL_RGB);
	Texture* mineBuilding = assetsRepository->getTexture("mineBuilding");
	Material* mineMaterial = new Material();
	mineMaterial->setAmbient(glm::vec3(0.2, 0.2, 0.2));
	mineMaterial->setDiffuse(glm::vec3(1.0, 1.0, 1.0));
	mineMaterial->setSpecular(glm::vec3(2.0, 2.0, 2.0));
	mineMaterial->setShininess(32.0f);
	Model mine;
	mine.setMatrix(glm::mat4(1.0));
	mine.setScale(glm::vec3(0.3));
	assetsRepository->registerMesh("mine", "models/mine.obj");
	Mesh* mineMesh = assetsRepository->getMesh("mine");
	mineMesh->setTexture(mineBuilding);
	mineMesh->setHeightmap(heightmap);
	mineMesh->setPosition(glm::vec2(0.0));
	mine.setMesh(mineMesh);
	mine.setMaterial(mineMaterial);
	mine.setProgram(buildingProgram);
	mine.name = "mine";
	models.push_back(mine);

	assetsRepository->registerTexture("houseBuilding", "textures/house.png", GL_RGB);
	Texture* houseBuilding = assetsRepository->getTexture("houseBuilding");
	Material* houseMaterial = new Material();
	houseMaterial->setAmbient(glm::vec3(0.2, 0.2, 0.2));
	houseMaterial->setDiffuse(glm::vec3(1.0, 1.0, 1.0));
	houseMaterial->setSpecular(glm::vec3(2.0, 2.0, 2.0));
	houseMaterial->setShininess(32.0f);
	Model house;
	house.setMatrix(glm::mat4(1.0));
	house.setScale(glm::vec3(0.3));
	assetsRepository->registerMesh("house", "models/house.obj");
	Mesh* houseMesh = assetsRepository->getMesh("house");
	houseMesh->setTexture(houseBuilding);
	houseMesh->setHeightmap(heightmap);
	houseMesh->setPosition(glm::vec2(0.0));
	house.setMesh(houseMesh);
	house.setMaterial(houseMaterial);
	house.setProgram(buildingProgram);
	house.name = "house";
	models.push_back(house);
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
	glutTimerFunc(1000.f / 60, FrameCallback, 0);
}

// ---------------------------------------------------
void Reshape(int width, int height)
{
	glViewport(0, 0, width, height);
	matProj = glm::perspective(glm::radians(90.0f), width / (float)height, 0.01f, 180.0f);
	float aspect = width / (float)height;
	float zoom = 5.0;
	minimapProj = glm::ortho(-aspect * zoom, aspect * zoom, -aspect * zoom, aspect * zoom, 0.01f, 100.0f);
	lightProj = glm::ortho(-100.0, 100.0, -100.0, 100.0, -100.0, 100.0);

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


void build() {
	if(validSpot(glm::vec2(worldPoint.x, worldPoint.z)) == 2) return;
	Model toBuildModel;
	int idx = 0;
	for (Model model : models) {
		if (idx != selected) {
			idx++;
			continue;
		}
		toBuildModel = model;
		idx++;
	}
	Model newModel;
	newModel.setMatrix(glm::translate(toBuildModel.getMatrix(), worldPoint));
	Mesh* newMesh = Mesh::clone(assetsRepository->getMesh(toBuildModel.name));
	newMesh->setPosition(glm::vec2(worldPoint.x, worldPoint.z));
	newModel.setScale(toBuildModel.getScale());
	newModel.setMesh(newMesh);
	newModel.setMaterial(toBuildModel.getMaterial());
	newModel.setProgram(toBuildModel.getProgram());
	newModel.name = toBuildModel.name;
	buildings.push_back(newModel);
	woodCount -= needWood(newModel.name);
	rockCount -= needRock(newModel.name);
	if (newModel.name == "castle") castleCount++;
	if (newModel.name == "sawmill") sawmillCount++;
	if (newModel.name == "mine") mineCount++;
	if (newModel.name == "house") houseCount++;
}
// ---------------------------------------------------
void Keyboard(unsigned char key, int x, int y)
{
	switch (key)
	{
		case '1':
			selected = 0;
			break;
		case '2':
			selected = 1;
			break;
		case '3':
			selected = 2;
			break;
		case '4':
			selected = 3;
			break;
		case 13:
			build();
			break;
		case 32:
			build();
			break;
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
	//glutAddMenuEntry("Standard", 501);
	glutAddMenuEntry("Grayscale", 502);
	glutAddMenuEntry("Negative", 503);
	glutAddMenuEntry("Sharpening", 504);
	glutAddMenuEntry("Blur", 505);
	glutAddMenuEntry("Edge", 506);
	glutAddMenuEntry("Vignette ", 507);

	int menu = glutCreateMenu(processMenu);
	glutAddSubMenu("Light", light);
	glutAddSubMenu("Fog", fog);
	//glutAddSubMenu("SkyBox", skyBox);
	glutAddSubMenu("Materials", materials);
	glutAddSubMenu("Minimapa", minimap);
	glutAddSubMenu("Postprocessy", postprocess);

	glutAttachMenu(GLUT_RIGHT_BUTTON);
}

void addResources(int value) {
	for (Model model : buildings) {
		std::cout << "name: " << model.name << std::endl;
		if (model.name == "mine") rockCount++;
		if (model.name == "sawmill") woodCount++;
	}
	glutTimerFunc(1000, addResources, 0);
}

// ---------------------------------------------------
int main(int argc, char* argv[])
{
	if (argc > 1) {
		main_seed = std::stoi(argv[1]);
	}

	// GLUT
	glutInit(&argc, argv);
	glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGBA | GLUT_DEPTH | GLUT_STENCIL);
	glutInitContextVersion(3, 2);
	glutInitContextProfile(GLUT_CORE_PROFILE);
	glutInitWindowSize(600, 600);
	glutCreateWindow("Medival builder - projekt OpenGL - Patryk Morawski");

	__CHECK_FOR_ERRORS;
	glutDisplayFunc(Render);
	glutReshapeFunc(Reshape);
	glutMouseFunc(MouseButton);
	glutMotionFunc(MouseMotion);
	glutMouseWheelFunc(MouseWheel);
	glutPassiveMotionFunc(PassiveMouseMotion);
	glutKeyboardFunc(Keyboard);
	glutSpecialFunc(SpecialKeys);
	glutTimerFunc(1000.0 / 60.0, FrameCallback, 0);
	glutTimerFunc(1000, addResources, 0);
	//createMenu();




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
