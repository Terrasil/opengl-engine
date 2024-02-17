#pragma once

#ifndef MODEL_H
#define MODEL_H

#include "mesh.h"
#include "program.h"

class Model
{
private:
	Mesh* mesh;
	Program* program; 
	glm::mat4 matrix;
public:
	void setMesh(Mesh* newMesh);
	Mesh* getMesh();
	void setProgram(Program* newProgram);
	Program* getProgram();
	void setMatrix(glm::mat4 newMatrix);
	glm::mat4 getMatrix();
	void draw(glm::vec3& cameraPos, glm::mat4& matProj, glm::mat4& matView);
	Model();
};

#endif 