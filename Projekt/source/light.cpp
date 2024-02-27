#include "light.h"
#include <gtc/matrix_transform.hpp>

void Light::setAmbient(glm::vec3 newAmbient) {
	ambient = newAmbient;
}

glm::vec3 Light::getAmbient() {
	return ambient;
}

void Light::setDiffuse(glm::vec3 newDiffuse) {
	diffuse = newDiffuse;
}

glm::vec3 Light::getDiffuse() {
	return diffuse;
}

void Light::setSpecular(glm::vec3 newSpecular) {
	specular = newSpecular;
}

glm::vec3 Light::getSpecular() {
	return specular;
}

void Light::setPosition(glm::vec3 newPosition) {
	position = newPosition;
}

glm::vec3 Light::getPosition() {
	return position;
}

void Light::setAttenuation(glm::vec3 newAttenuation) {
	attenuation = newAttenuation;
}

glm::vec3 Light::getAttenuation() {
	return attenuation;
}

void Light::setStrength(float newStrength) {
	strength = newStrength;
}

float Light::getStrength() {
	return strength;
}

void Light::setIsDirectional(bool newIsDirectional) {
	isDirectional = newIsDirectional;
}
bool Light::getIsDirectional() {
	return isDirectional;
}

void Light::setModel(Model* newModel) {
	model = newModel;
}
void Light::draw(glm::vec3& cameraPos, glm::mat4& matProj, glm::mat4& matView) {
	if (isDirectional) return;
	model->setMatrix(glm::translate(model->getMatrix(), position));
	model->setMatrix(glm::scale(model->getMatrix(), glm::vec3(0.25, 0.25, 0.25)));
	model->draw(cameraPos, matProj, matView, diffuse);
	model->setMatrix(glm::scale(model->getMatrix(), glm::vec3(4.0, 4.0, 4.0)));
	model->setMatrix(glm::translate(model->getMatrix(), -position));
}