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