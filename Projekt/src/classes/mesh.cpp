#include "mesh.h"

#include <iostream>
#include <gtc/type_ptr.hpp>
#include "obj_loader.hpp"
#include "settings.h"

void Mesh::load(std::string path) {
	if (!loadOBJ(path.c_str(), vertices, uvs, normals)) {
		std::cout << "Can't load obj file!" << std::endl;
	}
	else {
		loadVerticesToVBO();
		loadNormalsToVBO();
		loadUvsToVBO();
	}
}

void Mesh::setVertices(std::vector<glm::vec3> newVertices) {
	vertices = newVertices;
	loadVerticesToVBO();
}

void Mesh::setNormals(std::vector<glm::vec3> newNormals) {
	normals = newNormals;
	loadNormalsToVBO();
}

void Mesh::setUvs(std::vector<glm::vec2> newUvs) {
	uvs = newUvs;
	loadUvsToVBO();
}

void Mesh::loadVerticesToVBO() {
	glBindVertexArray(VAO);
	glGenBuffers(1, &verticesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &(vertices.at(0)), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

void Mesh::loadUvsToVBO() {
	glBindVertexArray(VAO);
	glGenBuffers(1, &uvsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, uvsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * uvs.size(), &(uvs.at(0)), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
}

void Mesh::loadNormalsToVBO() {
	glBindVertexArray(VAO);
	glGenBuffers(1, &normalsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), &(normals.at(0)), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}

void Mesh::setTexture(Texture* newTexture) {
	texture = newTexture;
}

void Mesh::setMaterial(Material* newMaterial) {
	material = newMaterial;
}

void Mesh::draw(Program* program, glm::mat4& matrix, glm::vec3& cameraPos, glm::mat4& matProj, glm::mat4& matView) {
	glUseProgram(program->getId());
	glUniformMatrix4fv(glGetUniformLocation(program->getId(), "matProj"), 1, GL_FALSE, glm::value_ptr(matProj));
	glUniformMatrix4fv(glGetUniformLocation(program->getId(), "matView"), 1, GL_FALSE, glm::value_ptr(matView));
	glUniform3fv(glGetUniformLocation(program->getId(), "cameraPos"), 1, &cameraPos[0]);
	glUniformMatrix4fv(glGetUniformLocation(program->getId(), "matModel"), 1, GL_FALSE, glm::value_ptr(matrix));

	bool useTexture = (texture != nullptr && texture->getId() != NULL);

	if (useTexture) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->getId());
		glUniform1i(glGetUniformLocation(program->getId(), "tex"), 0);
	}
	//Renderer::getInstance()
	glUniform1i(glGetUniformLocation(program->getId(), "useTexture"), useTexture);
	glUniform1i(glGetUniformLocation(program->getId(), "lightVisibility"), Settings::getInstance().lightVisibility);
	glUniform1i(glGetUniformLocation(program->getId(), "lightMode"), Settings::getInstance().lightMode);
	glUniform1i(glGetUniformLocation(program->getId(), "lightQuality"), Settings::getInstance().lightQuality);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

Mesh::Mesh() {
	glGenVertexArrays(1, &VAO);
}