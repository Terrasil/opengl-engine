#include "model.h"

#include <gtc/type_ptr.hpp>

void Model::setMesh(Mesh* newMesh) {
	mesh = newMesh;
}

Mesh* Model::getMesh() {
	return mesh;
}

void Model::setProgram(Program* newProgram) {
	program = newProgram;
}

Program* Model::getProgram() {
	return program;
}

void Model::setMatrix(glm::mat4 newMatrix) {
	matrix = newMatrix;
}

glm::mat4 Model::getMatrix() {
	return matrix;
}

void Model::draw(glm::vec3& cameraPos, glm::mat4& matProj, glm::mat4& matView) {
	mesh->draw(program, matrix, cameraPos, matProj, matView);
}

Model::Model() {
	matrix = glm::mat4(1.0);
}