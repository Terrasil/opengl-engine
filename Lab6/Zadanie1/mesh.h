#pragma once

#ifndef MESH_H
#define MESH_H

#include <vector>
#include <glm.hpp>
#include <string>
#include <glew.h>
#include "program.h"
#include "texture.h"
#include "material.h"

class Mesh
{
private:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals; 
	GLuint VAO;
	GLuint verticesVBO;
	GLuint uvsVBO;
	GLuint normalsVBO;
	Texture* texture;
	void loadVerticesToVBO();
	void loadUvsToVBO();
	void loadNormalsToVBO();
public:
	void load(std::string path);
	void setVertices(std::vector<glm::vec3> newVertices);
	void setUvs(std::vector<glm::vec2> newUvs);
	void setNormals(std::vector<glm::vec3> newNormals);
	void setTexture(Texture* newTexture);
	Texture* getTexture();
	std::vector<glm::vec3> getVertices();
	void draw(Program* program, glm::mat4& matrix, glm::vec3& cameraPos, glm::mat4& matProj, glm::mat4& matView, Material* material);
	Mesh();
};

#endif 