#pragma once

#ifndef LIGHT_H
#define LIGHT_H

#include <glm.hpp>
#include <string>

#include "model.h"

class Light
{
private:
	glm::vec3 ambient;
	glm::vec3 specular;
	glm::vec3 position;
	glm::vec3 diffuse;
	glm::vec3 attenuation;
	bool isDirectional;
	Model* model;
	float strength;
public:
	void setAmbient(glm::vec3 newAmbient);
	glm::vec3 getAmbient();
	void setDiffuse(glm::vec3 newDiffuse);
	glm::vec3 getDiffuse();
	void setSpecular(glm::vec3 newSpecular);
	glm::vec3 getSpecular();
	void setPosition(glm::vec3 newPosition);
	glm::vec3 getPosition();
	void setAttenuation(glm::vec3 newAttenuation);
	glm::vec3 getAttenuation();
	void setStrength(float newStrength);
	float getStrength();
	void setIsDirectional(bool newIsDirectional);
	bool getIsDirectional();
	void setModel(Model* newModel);
	void draw(glm::vec3& cameraPos, glm::mat4& matProj, glm::mat4& matView);
};

#endif 