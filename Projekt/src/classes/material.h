#pragma once

#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm.hpp>

class Material
{
private:
	float shininess;	//Ns
	glm::vec3 ambient;	//Ka
	glm::vec3 diffuse;	//Kd
	glm::vec3 specular;	//Ks
	glm::vec3 emissive;	//Ke
	float density;		//Ni
	float dissolve;		//d
	int illumination;	//illum
public:
	void setShininess(float newShininess);
	float getShininess();
	void setAmbient(glm::vec3 newAmbient);
	glm::vec3 getAmbient();
	void setDiffuse(glm::vec3 newDiffuse);
	glm::vec3 getDiffuse();
	void setSpecular(glm::vec3 newSpecular);
	glm::vec3 getSpecular();
	void setEmissive(glm::vec3 newEmissive);
	glm::vec3 getEmissive();
	void setDensity(float newDensity);
	float getDensity();
	void setDissolve(float newDissolve);
	float getDissolve();
	void setIllumination(int newIllumination);
	int getIllumination();
};

#endif 
