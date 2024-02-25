#pragma once

#ifndef TERRAIN_H
#define TERRAIN_H

#include <vector>
#include <glm.hpp>
#include <string>
#include <glew.h>
#include "program.h"
#include "texture.h"
#include "material.h"

class Terrain
{
private:
	std::vector<glm::vec3> vertices;
	std::vector<glm::vec2> uvs;
	std::vector<glm::vec3> normals;
	GLuint VAO;
	GLuint verticesVBO;
	GLuint uvsVBO;
	GLuint normalsVBO;
	Material* material = new Material();
	Texture* texture1;
	Texture* texture2;
	Texture* texture3;
	Texture* heightmap;
	Program* program;
	void loadVerticesToVBO();
	void loadUvsToVBO();
	void loadNormalsToVBO();
public:
	void load(std::string path);
	void setVertices(std::vector<glm::vec3> newVertices);
	void setUvs(std::vector<glm::vec2> newUvs);
	void setNormals(std::vector<glm::vec3> newNormals);
	void setTexture1(Texture* newTexture1);
	void setTexture2(Texture* newTexture2);
	void setTexture3(Texture* newTexture3);
	void setHeightmap(Texture* newHeightmap);
	Texture* getTexture1();
	Texture* getTexture2();
	Texture* getTexture3();
	Texture* getHeightmap();
	void setMaterial(Material* newMaterial);
	void setProgram(Program* newProgram);
	Program* getProgram();
	std::vector<glm::vec3> getVertices();
	void draw(glm::vec3& cameraPos, glm::mat4& matProj, glm::mat4& matView);
	Terrain();
};

#endif 