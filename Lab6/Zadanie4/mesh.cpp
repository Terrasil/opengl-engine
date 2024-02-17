#include "mesh.h"

#include <iostream>
#include <gtc/type_ptr.hpp>
#include "obj_loader.hpp"
#include "settings.h"
#include "renderer.h"

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

Texture* Mesh::getTexture() {
	return texture;
}

std::vector<glm::vec3> Mesh::getVertices() {
	return vertices;
}

void Mesh::draw(Program* program, glm::mat4& matrix, glm::vec3& cameraPos, glm::mat4& matProj, glm::mat4& matView, Material* material, glm::vec3& color) {
	GLuint programId = program->getId();
	glUseProgram(programId);
	glUniformMatrix4fv(glGetUniformLocation(programId, "matProj"), 1, GL_FALSE, glm::value_ptr(matProj));
	glUniformMatrix4fv(glGetUniformLocation(programId, "matView"), 1, GL_FALSE, glm::value_ptr(matView));
	glUniformMatrix4fv(glGetUniformLocation(programId, "matModel"), 1, GL_FALSE, glm::value_ptr(matrix));
	glUniform3fv(glGetUniformLocation(programId, "cameraPos"), 1, &cameraPos[0]);
	glUniform3fv(glGetUniformLocation(programId, "color"), 1, &color[0]);

	bool useTexture = (texture != nullptr && texture->getId() != NULL);
	if (useTexture) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture->getId());
		glUniform1i(glGetUniformLocation(programId, "tex"), 0);
	}
	glUniform1i(glGetUniformLocation(programId, "useTexture"), useTexture);
	
	bool useMaterial = (material != nullptr);
	if (useMaterial) {
		glUniform3fv(glGetUniformLocation(programId, "material.Ambient"), 1, glm::value_ptr(material->getAmbient()));
		glUniform3fv(glGetUniformLocation(programId, "material.Diffuse"), 1, glm::value_ptr(material->getDiffuse()));
		glUniform3fv(glGetUniformLocation(programId, "material.Specular"), 1, glm::value_ptr(material->getSpecular()));
		glUniform1f(glGetUniformLocation(programId, "material.Shininess"), material->getShininess());
	}

	for (int i = 0; i < Renderer::getInstance().getLights().size() && i < 256; i++) {
		Light* light = Renderer::getInstance().getLights().at(i);
		light->setIsDirectional(Settings::getInstance().directional);
		if (light->getIsDirectional()) {
			glUniform3fv(glGetUniformLocation(programId, ("dirLight[" + std::to_string(i) + "].Ambient").c_str()), 1, glm::value_ptr(light->getAmbient()));
			glUniform3fv(glGetUniformLocation(programId, ("dirLight[" + std::to_string(i) + "].Diffuse").c_str()), 1, glm::value_ptr(light->getDiffuse()));
			glUniform3fv(glGetUniformLocation(programId, ("dirLight[" + std::to_string(i) + "].Specular").c_str()), 1, glm::value_ptr(light->getSpecular()));
			glUniform3fv(glGetUniformLocation(programId, ("dirLight[" + std::to_string(i) + "].Attenuation").c_str()), 1, glm::value_ptr(light->getAttenuation()));
			glUniform3fv(glGetUniformLocation(programId, ("dirLight[" + std::to_string(i) + "].Position").c_str()), 1, glm::value_ptr(light->getPosition()));
		} else {
			glUniform3fv(glGetUniformLocation(programId, ("light[" + std::to_string(i) + "].Ambient").c_str()), 1, glm::value_ptr(light->getAmbient()));
			glUniform3fv(glGetUniformLocation(programId, ("light[" + std::to_string(i) + "].Diffuse").c_str()), 1, glm::value_ptr(light->getDiffuse()));
			glUniform3fv(glGetUniformLocation(programId, ("light[" + std::to_string(i) + "].Specular").c_str()), 1, glm::value_ptr(light->getSpecular()));
			glUniform3fv(glGetUniformLocation(programId, ("light[" + std::to_string(i) + "].Attenuation").c_str()), 1, glm::value_ptr(light->getAttenuation()));
			glUniform3fv(glGetUniformLocation(programId, ("light[" + std::to_string(i) + "].Position").c_str()), 1, glm::value_ptr(light->getPosition()));
		}
	}
	
	glUniform1i(glGetUniformLocation(programId, "lightVisibility"), Settings::getInstance().lightVisibility);
	glUniform1i(glGetUniformLocation(programId, "lightMode"), Settings::getInstance().lightMode);
	glUniform1i(glGetUniformLocation(programId, "lightQuality"), Settings::getInstance().lightQuality);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

Mesh::Mesh() {
	glGenVertexArrays(1, &VAO);
}