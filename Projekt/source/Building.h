#pragma once

#ifndef BUILDING_H
#define BUILDING_H

#include <glm.hpp>
#include "model.h"
class Building
{
private:
	Model* model;
	glm::vec3 position;
	glm::vec3 rotation;
	int state;
public:
	void draw(glm::vec3& cameraPos, glm::mat4& matProj, glm::mat4& matView);
	void draw(Program* program, glm::vec3& cameraPos, glm::mat4& matProj, glm::mat4& matView);
	void draw(glm::vec3& cameraPos, glm::mat4& matProj, glm::mat4& matView, glm::vec3& color);
};

#endif