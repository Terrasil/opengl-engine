#include "light.h"

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