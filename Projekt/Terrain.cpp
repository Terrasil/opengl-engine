#include "Terrain.h"

#include <iostream>
#include <gtc/type_ptr.hpp>
#include "obj_loader.hpp"
#include "settings.h"
#include "renderer.h"

void Terrain::load(std::string path) {
	if (!loadOBJ(path.c_str(), vertices, uvs, normals)) {
		std::cout << "Can't load obj file!" << std::endl;
	}
	else {
		loadVerticesToVBO();
		loadNormalsToVBO();
		loadUvsToVBO();
	}
}

void Terrain::setVertices(std::vector<glm::vec3> newVertices) {
	vertices = newVertices;
	loadVerticesToVBO();
}

void Terrain::setNormals(std::vector<glm::vec3> newNormals) {
	normals = newNormals;
	loadNormalsToVBO();
}

void Terrain::setUvs(std::vector<glm::vec2> newUvs) {
	uvs = newUvs;
	loadUvsToVBO();
}

void Terrain::loadVerticesToVBO() {
	glBindVertexArray(VAO);
	glGenBuffers(1, &verticesVBO);
	glBindBuffer(GL_ARRAY_BUFFER, verticesVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * vertices.size(), &(vertices.at(0)), GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);
}

void Terrain::loadUvsToVBO() {
	glBindVertexArray(VAO);
	glGenBuffers(1, &uvsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, uvsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec2) * uvs.size(), &(uvs.at(0)), GL_STATIC_DRAW);
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(2);
	glBindVertexArray(0);
}

void Terrain::loadNormalsToVBO() {
	glBindVertexArray(VAO);
	glGenBuffers(1, &normalsVBO);
	glBindBuffer(GL_ARRAY_BUFFER, normalsVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(glm::vec3) * normals.size(), &(normals.at(0)), GL_STATIC_DRAW);
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(1);
	glBindVertexArray(0);
}

void Terrain::setTexture1(Texture* newTexture1) {
	texture1 = newTexture1;
}

void Terrain::setTexture2(Texture* newTexture2) {
	texture2 = newTexture2;
}

void Terrain::setTexture3(Texture* newTexture3) {
	texture3 = newTexture3;
}

void Terrain::setHeightmap(Texture* newHeightmap) {
	heightmap = newHeightmap;
}

Texture* Terrain::getTexture1() {
	return texture1;
}

Texture* Terrain::getTexture2() {
	return texture2;
}

Texture* Terrain::getTexture3() {
	return texture3;
}

Texture* Terrain::getHeightmap() {
	return heightmap;
}

std::vector<glm::vec3> Terrain::getVertices() {
	return vertices;
}

void Terrain::setProgram(Program* newProgram) {
	program = newProgram;
}

Program* Terrain::getProgram() {
	return program;
}

void Terrain::setMaterial(Material* newMaterial) {
	material = newMaterial;
}

void Terrain::draw(glm::vec3& cameraPos, glm::mat4& matProj, glm::mat4& matView) {
	GLuint programId = program->getId();
	glm::mat4 matrix = glm::mat4(1.0);
	matrix = glm::scale(matrix, glm::vec3(0.25));
	glUseProgram(programId);
	glUniformMatrix4fv(glGetUniformLocation(programId, "matProj"), 1, GL_FALSE, glm::value_ptr(matProj));
	glUniformMatrix4fv(glGetUniformLocation(programId, "matView"), 1, GL_FALSE, glm::value_ptr(matView));
	glUniformMatrix4fv(glGetUniformLocation(programId, "matModel"), 1, GL_FALSE, glm::value_ptr(matrix));
	glUniform3fv(glGetUniformLocation(programId, "cameraPos"), 1, &cameraPos[0]);
	//glUniform3fv(glGetUniformLocation(programId, "color"), 1, &color[0]);

	bool useTexture1 = (texture1 != nullptr && texture1->getId() != NULL);
	bool useTexture2 = (texture2 != nullptr && texture2->getId() != NULL);
	bool useTexture3 = (texture3 != nullptr && texture3->getId() != NULL);
	bool useHeightmapTexture = (heightmap != nullptr && heightmap->getId() != NULL);
	bool useTexture = useTexture1 && useTexture2 && useTexture3 && useHeightmapTexture;
	if (useTexture) {
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1->getId());
		glUniform1i(glGetUniformLocation(programId, "tex1"), 0);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2->getId());
		glUniform1i(glGetUniformLocation(programId, "tex2"), 1);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, texture3->getId());
		glUniform1i(glGetUniformLocation(programId, "tex3"), 2);
		glActiveTexture(GL_TEXTURE3);
		glBindTexture(GL_TEXTURE_2D, heightmap->getId());
		glUniform1i(glGetUniformLocation(programId, "heightmap"), 3);
	}
	glUniform1i(glGetUniformLocation(programId, "useTexture"), useTexture);
	glUniformMatrix4fv(glGetUniformLocation(programId, "lightProj"), 1, GL_FALSE, glm::value_ptr(Renderer::getInstance().getLightProj()));
	glUniformMatrix4fv(glGetUniformLocation(programId, "lightView"), 1, GL_FALSE, glm::value_ptr(Renderer::getInstance().getLightView()));
	glActiveTexture(GL_TEXTURE4);
	glBindTexture(GL_TEXTURE_2D, Renderer::getInstance().getShadowTexture()->getId());
	glUniform1i(glGetUniformLocation(programId, "shadowMap"), 4);

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


	glActiveTexture(GL_TEXTURE5);
	glBindTexture(GL_TEXTURE_CUBE_MAP, Renderer::getInstance().getSkyBoxes().at(Settings::getInstance().skyBox)->getTextureId());
	glUniform1i(glGetUniformLocation(programId, "tex_skybox"), 5);

	glUniform1i(glGetUniformLocation(programId, "lightVisibility"), Settings::getInstance().lightVisibility);
	glUniform1i(glGetUniformLocation(programId, "lightMode"), Settings::getInstance().lightMode);
	glUniform1i(glGetUniformLocation(programId, "lightQuality"), Settings::getInstance().lightQuality);
	glUniform1i(glGetUniformLocation(programId, "fogVisibility"), Settings::getInstance().fogVisibility);
	glUniform1i(glGetUniformLocation(programId, "fogVisibility"), Settings::getInstance().fogVisibility);
	glUniform1i(glGetUniformLocation(programId, "showRefraction"), Settings::getInstance().showRefraction);
	glUniform1i(glGetUniformLocation(programId, "showReflection"), Settings::getInstance().showReflection);

	glBindVertexArray(VAO);
	glDrawArrays(GL_TRIANGLES, 0, vertices.size());
}

Terrain::Terrain() {
	glGenVertexArrays(1, &VAO);
}