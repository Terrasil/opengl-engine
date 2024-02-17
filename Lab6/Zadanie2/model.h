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
public:
	std::string name;
	void setMesh(Mesh* newMesh);
	Mesh* getMesh();
	void setProgram(Program* newProgram);
	Program* getProgram();
	void setMatrix(glm::mat4 newMatrix);
	glm::mat4 getMatrix();
	void setMaterial(Material* newMaterial);
	void draw(glm::vec3& cameraPos, glm::mat4& matProj, glm::mat4& matView);
	Model();
};

#endif 