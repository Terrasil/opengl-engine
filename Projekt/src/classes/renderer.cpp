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