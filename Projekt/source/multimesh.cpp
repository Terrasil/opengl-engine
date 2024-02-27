#include "multimesh.h"

#include <iostream>
#include <gtc/type_ptr.hpp>
#include "obj_loader.hpp"
#include "settings.h"
#include "renderer.h"

void MultiMesh::load(std::string path) {
	if (!loadOBJ(path.c_str(), vertices, uvs, normals)) {
		std::cout << "Can't load obj file!" << std::endl;
	}
	else {
		loadVerticesToVBO();
		loadNormalsToVBO();
		loadUvsToVBO();
	}
}

void MultiMesh::setVertices(std::vector<glm::vec3> newVertices) {
	vertices = newVertices;
	loadVerticesToVBO();
}

void MultiMesh::setNormals(std::vector<glm::vec3> newNormals) {
	normals = newNormals;
	loadNormalsToVBO();
}

void MultiMesh::setUvs(std::vector<glm::vec2> newUvs) {
	uvs = newUvs;
	loadUvsToVBO();
}

void MultiMesh::setPositions(std::vector<glm::vec2> newPositions) {
	positions = newPositions;
	loadPositionsToVBO();
}

void MultiMesh::loadVerticesToVBO() {
	glBindVertexArray(VAO);
	glGenBuffers(1, &verticesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &(vertices.at(0)), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

void MultiMesh::loadUvsToVBO() {
	glBindVertexArray(VAO);
	glGenBuffers(1, &uvsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, uvsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * uvs.size(), &(uvs.at(0)), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
}

void MultiMesh::loadPositionsToVBO() {
	glBindVertexArray(VAO);
	glGenBuffers(1, &positionsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, positionsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * positions.size(), &(positions.at(0)), GL_STATIC_DRAW);
	glVertexAttribPointer(3, 2, GL_FLOAT, GL_FALSE, 0, NULL); 
	glVertexAttribDivisor(3, 1);
	glEnableVertexAttribArray(3);
	glBindVertexArray(0);
}

void MultiMesh::loadNormalsToVBO() {
	glBindVertexArray(VAO);
	glGenBuffers(1, &normalsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), &(normals.at(0)), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}

void MultiMesh::loadMatrixesToVBO() {
	glBindVertexArray(VAO);
	glGenBuffers(1, &matrixesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, matrixesVBO);
	glBufferData(GL_ARRAY_BUFFER, matrixes.size() * sizeof(glm::mat4), &matrixes[0], GL_STATIC_DRAW);
	glVertexAttribPointer(4, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)0);
	glEnableVertexAttribArray(4);
	glVertexAttribPointer(5, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(sizeof(glm::vec4)));
	glEnableVertexAttribArray(5);
	glVertexAttribPointer(6, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(2 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(6);
	glVertexAttribPointer(7, 4, GL_FLOAT, GL_FALSE, sizeof(glm::mat4), (void*)(3 * sizeof(glm::vec4)));
	glEnableVertexAttribArray(7);
	glVertexAttribDivisor(4, 1);
	glVertexAttribDivisor(5, 1);
	glVertexAttribDivisor(6, 1);
	glVertexAttribDivisor(7, 1);
}

void MultiMesh::setTexture(Texture* newTexture) {
	texture = newTexture;
}

Texture* MultiMesh::getTexture() {
	return texture;
}

std::vector<glm::vec3> MultiMesh::getVertices() {
	return vertices;
}

void MultiMesh::setMatrixes(std::vector<glm::mat4x4> newMatrixes) {
	matrixes = newMatrixes;
	loadMatrixesToVBO();
}


void MultiMesh::setHeightmap(Texture* newHeightmap) {
	heightmap = newHeightmap;
}
Texture* MultiMesh::getHeightmap() {
	return heightmap;
}

void MultiMesh::draw(Program* program, glm::mat4& matrix, glm::vec3& cameraPos, glm::mat4& matProj, glm::mat4& matView, Material* material, glm::vec3& color) {
	GLuint programId = program->getId();
	glUseProgram(programId);
	glUniformMatrix4fv(glGetUniformLocation(programId, "matProj"), 1, GL_FALSE, glm::value_ptr(matProj));
	glUniformMatrix4fv(glGetUniformLocation(programId, "matView"), 1, GL_FALSE, glm::value_ptr(matView));
	glUniformMatrix4fv(glGetUniformLocation(programId, "matModel"), 1, GL_FALSE, glm::value_ptr(matrix));
	glUniform3fv(glGetUniformLocation(programId, "cameraPos"), 1, &cameraPos[0]);
	glUniform3fv(glGetUniformLocation(programId, "color"), 1, &color[0]);
	glUniform1f(glGetUniformLocation(programId, "time"), Renderer::getInstance().getTime());

	bool useTexture = (texture != nullptr && texture->getId() != NULL);
	if (useTexture) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->getId());
		glUniform1i(glGetUniformLocation(programId, "tex"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, heightmap->getId());
		glUniform1i(glGetUniformLocation(programId, "heightmap"), 1);
	}
	glUniform1i(glGetUniformLocation(programId, "useTexture"), useTexture);

	bool useMaterial = (material != nullptr);
	if (useMaterial) {
		glUniform3fv(glGetUniformLocation(programId, "material.Ambient"), 1, glm::value_ptr(material->getAmbient()));
		glUniform3fv(glGetUniformLocation(programId, "material.Diffuse"), 1, glm::value_ptr(material->getDiffuse()));
		glUniform3fv(glGetUniformLocation(programId, "material.Specular"), 1, glm::value_ptr(material->getSpecular()));
		glUniform1f(glGetUniformLocation(programId, "material.Shininess"), material->getShininess());
		glUniform1f(glGetUniformLocation(programId, "material.Refraction"), material->getRefraction());
		glUniform1f(glGetUniformLocation(programId, "material.RefractionCoefficient"), material->getRefractionCoefficient());
		glUniform1f(glGetUniformLocation(programId, "material.Reflection"), material->getReflection());
	}

	for (int i = 0; i < Renderer::getInstance().getLights().size() && i < 32; i++) {
		Light* light = Renderer::getInstance().getLights().at(i);
		if (light->getIsDirectional()) {
			glUniform3fv(glGetUniformLocation(programId, ("dirLight[" + std::to_string(i) + "].Ambient").c_str()), 1, glm::value_ptr(light->getAmbient()));
			glUniform3fv(glGetUniformLocation(programId, ("dirLight[" + std::to_string(i) + "].Diffuse").c_str()), 1, glm::value_ptr(light->getDiffuse()));
			glUniform3fv(glGetUniformLocation(programId, ("dirLight[" + std::to_string(i) + "].Specular").c_str()), 1, glm::value_ptr(light->getSpecular()));
			glUniform3fv(glGetUniformLocation(programId, ("dirLight[" + std::to_string(i) + "].Attenuation").c_str()), 1, glm::value_ptr(light->getAttenuation()));
			glUniform3fv(glGetUniformLocation(programId, ("dirLight[" + std::to_string(i) + "].Position").c_str()), 1, glm::value_ptr(light->getPosition()));
		}
		else {
			glUniform3fv(glGetUniformLocation(programId, ("light[" + std::to_string(i) + "].Ambient").c_str()), 1, glm::value_ptr(light->getAmbient()));
			glUniform3fv(glGetUniformLocation(programId, ("light[" + std::to_string(i) + "].Diffuse").c_str()), 1, glm::value_ptr(light->getDiffuse()));
			glUniform3fv(glGetUniformLocation(programId, ("light[" + std::to_string(i) + "].Specular").c_str()), 1, glm::value_ptr(light->getSpecular()));
			glUniform3fv(glGetUniformLocation(programId, ("light[" + std::to_string(i) + "].Attenuation").c_str()), 1, glm::value_ptr(light->getAttenuation()));
			glUniform3fv(glGetUniformLocation(programId, ("light[" + std::to_string(i) + "].Position").c_str()), 1, glm::value_ptr(light->getPosition()));
		}
	}


	glActiveTexture(GL_TEXTURE2);
	glBindTexture(GL_TEXTURE_CUBE_MAP, Renderer::getInstance().getSkyBoxes().at(Settings::getInstance().skyBox)->getTextureId());
	glUniform1i(glGetUniformLocation(programId, "tex_skybox"), 2);

	glUniform1i(glGetUniformLocation(programId, "lightVisibility"), Settings::getInstance().lightVisibility);
	glUniform1i(glGetUniformLocation(programId, "lightMode"), Settings::getInstance().lightMode);
	glUniform1i(glGetUniformLocation(programId, "lightQuality"), Settings::getInstance().lightQuality);
	glUniform1i(glGetUniformLocation(programId, "fogVisibility"), Settings::getInstance().fogVisibility);
	glUniform1i(glGetUniformLocation(programId, "fogVisibility"), Settings::getInstance().fogVisibility);
	glUniform1i(glGetUniformLocation(programId, "showRefraction"), Settings::getInstance().showRefraction);
	glUniform1i(glGetUniformLocation(programId, "showReflection"), Settings::getInstance().showReflection);

	glBindVertexArray(VAO);
	glDrawArraysInstanced(GL_TRIANGLES, 0, vertices.size(), matrixes.size());
}

MultiMesh::MultiMesh() {
	glGenVertexArrays(1, &VAO);
}