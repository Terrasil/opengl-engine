#pragma once

#ifndef MATERIAL_H
#define MATERIAL_H

#include <glm.hpp>

class Material
{
private:
	float shininess = 1.0;				//Ns
	glm::vec3 ambient = glm::vec3(0.0);	//Ka
	glm::vec3 diffuse = glm::vec3(0.0); //Kd
	glm::vec3 specular = glm::vec3(0.0);//Ks
	glm::vec3 emissive = glm::vec3(0.0);//Ke
	float density = 1.0;				//Ni
	float dissolve = 1.0;				//d
	int illumination = 0;				//illum
	float refraction = 0.0;
	float refractionCoefficient = 0.0;
	float reflection = 0.0;
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
	void setRefraction(float newRefraction);
	float getRefraction();
	void setRefractionCoefficient(float newRefractionCoefficient);
	float getRefractionCoefficient();
	void setReflection(float newReflection);
	float getReflection();
};

#endif 
