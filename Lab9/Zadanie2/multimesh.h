#pragma once

#ifndef MULTIMESH_H
#define MULTIMESH_H

#include <vector>
#include <glm.hpp>
#include <string>
#include <glew.h>
#include "program.h"
#include "texture.h"
#include "material.h"

class MultiMesh
{
private:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	std::vector<glm::mat4x4> matrixes;
	GLuint VAO;
	GLuint verticesVBO;
	GLuint uvsVBO;
	GLuint normalsVBO;
	GLuint matrixesVBO;
	Texture* texture;
	void loadVerticesToVBO();
	void loadUvsToVBO();
	void loadNormalsToVBO();
	void loadMatrixesToVBO();
public:
	void load(std::string path);
	void setVertices(std::vector<glm::vec3> newVertices);
	void setUvs(std::vector<glm::vec2> newUvs);
	void setNormals(std::vector<glm::vec3> newNormals);
	void setTexture(Texture* newTexture);
	Texture* getTexture();
	std::vector<glm::vec3> getVertices();
	void setMatrixes(std::vector<glm::mat4> newMatrixes);
	void draw(Program* program, glm::mat4& matrix, glm::vec3& cameraPos, glm::mat4& matProj, glm::mat4& matView, Material* material, glm::vec3& color);
	MultiMesh();
};

#endif 
