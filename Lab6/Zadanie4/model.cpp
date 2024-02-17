#include "model.h"

#include <gtc/type_ptr.hpp>
#include <iostream>
#include "material.h"
#include "renderer.h"
#include "light.h"

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
	/*std::cout << "y" << std::endl;
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			std::cout << matrix[i][j] << " ";
		}
		std::cout << std::endl;
	}
	for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			std::cout << newMatrix[i][j] << " ";
		}
		std::cout << std::endl;
	}*/
	matrix = newMatrix;
	/*for (int i = 0; i < 4; ++i) {
		for (int j = 0; j < 4; ++j) {
			std::cout << matrix[i][j] << " ";
		}
		std::cout << std::endl;
	}*/
}

glm::mat4 Model::getMatrix() {
	return matrix;
}

void Model::setMaterial(Material* newMaterial) {
	material = newMaterial;
}

void Model::draw(glm::vec3& cameraPos, glm::mat4& matProj, glm::mat4& matView) {
	glm::vec3 color = glm::vec3(0.0);
	mesh->draw(program, matrix, cameraPos, matProj, matView, material, color);
}
void Model::draw(glm::vec3& cameraPos, glm::mat4& matProj, glm::mat4& matView, glm::vec3& color) {
	mesh->draw(program, matrix, cameraPos, matProj, matView, material, color);
}

Model::Model() {
	matrix = glm::mat4(1.0);
}