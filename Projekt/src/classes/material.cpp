#include "material.h"

void Material::setShininess(float newShininess) {
	shininess = newShininess;
}

float Material::getShininess() {
	return shininess;
}

void Material::setAmbient(glm::vec3 newAmbient) {
	ambient = newAmbient;
}

glm::vec3 Material::getAmbient() {
	return ambient;
}

void Material::setDiffuse(glm::vec3 newDiffuse) {
	diffuse = newDiffuse;
}

glm::vec3 Material::getDiffuse() {
	return diffuse;
}

void Material::setSpecular(glm::vec3 newSpecular) {
	specular = newSpecular;
}

glm::vec3 Material::getSpecular() {
	return specular;
}

void Material::setEmissive(glm::vec3 newEmissive) {
	emissive = newEmissive;
}

glm::vec3 Material::getEmissive() {
	return emissive;
}

void Material::setDensity(float newDensity) {
	density = newDensity;
}

float Material::getDensity() {
	return density;
}

void Material::setDissolve(float newDissolve) {
	dissolve = newDissolve;
}

float Material::getDissolve() {
	return dissolve;
}

void Material::setIllumination(int newIllumination) {
	illumination = newIllumination;
}

int Material::getIllumination() {
	return illumination;
}