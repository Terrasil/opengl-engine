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
	glm::vec2 position;
	GLuint VAO;
	GLuint verticesVBO;
	GLuint uvsVBO;
	GLuint normalsVBO;
	Texture* texture;
	Texture* heightmap;
	int state = 0;
	void loadVerticesToVBO();
	void loadUvsToVBO();
	void loadNormalsToVBO();
public:
	void load(std::string path);
	void setVertices(std::vector<glm::vec3> newVertices);
	void setUvs(std::vector<glm::vec2> newUvs);
	void setNormals(std::vector<glm::vec3> newNormals);
	void setTexture(Texture* newTexture);
	void setHeightmap(Texture* newHeightmap);
	void setPosition(glm::vec2 newPosition);
	void setState(int newState);
	int getState();
	glm::vec2 getPosition();
	Texture* getTexture();
	std::vector<glm::vec3> getVertices();
	void draw(Program* program, glm::mat4& matrix, glm::vec3& cameraPos, glm::mat4& matProj, glm::mat4& matView, Material* material, glm::vec3& color, glm::vec3& scale);
	static Mesh* clone(const Mesh* original);
	Mesh();
};

#endif 