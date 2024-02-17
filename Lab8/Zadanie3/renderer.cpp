#include "renderer.h"

Renderer::Renderer() {
}

Renderer::~Renderer() {
}

Renderer& Renderer::getInstance() {
	static Renderer instance;
	return instance;
}

std::vector<Light*> Renderer::getLights() {
	return lights;
}

void Renderer::addLights(Light* light) {
	lights.push_back(light);
}

std::vector<SkyBox*> Renderer::getSkyBoxes() {
	return skyboxes;
}

void Renderer::addSkyBox(SkyBox* skybox) {
	skyboxes.push_back(skybox);
}

void Renderer::setMultiMesh(MultiMesh* newMultiMesh) {
	multiMesh = newMultiMesh;
}

MultiMesh* Renderer::getMultiMesh() {
	return multiMesh;
}

void Renderer::increaseTime(double add) {
	time += add;
}

void Renderer::setTime(double newTime) {
	time = newTime;
}

double Renderer::getTime() {
	return time;
}