#pragma once

#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"
#include "program.h"

class Model
{
private:
	Mesh* mesh;
	Material* material;
	Program* program; 
	glm::mat4 matrix;
	glm::vec3 scale;
	float rotation;
public:
	std::string name;
	void setMesh(Mesh* newMesh);
	Mesh* getMesh();
	void setProgram(Program* newProgram);
	Program* getProgram();
	void setMatrix(glm::mat4 newMatrix);
	glm::mat4 getMatrix();
	void setMaterial(Material* newMaterial);
	Material* getMaterial();
	void setPosition(glm::vec2 newPosition);
	glm::vec2 getPosition();
	void setScale(glm::vec3 newScale);
	glm::vec3 getScale();
	void setRotation(float newRotation);
	float getRotation();
	void draw(glm::vec3& cameraPos, glm::mat4& matProj, glm::mat4& matView);
	void draw(Program* program, glm::vec3& cameraPos, glm::mat4& matProj, glm::mat4& matView);
	void draw(glm::vec3& cameraPos, glm::mat4& matProj, glm::mat4& matView, glm::vec3& color);
	Model();
};

#endif 