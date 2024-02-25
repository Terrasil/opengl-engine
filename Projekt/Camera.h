#pragma once

#ifndef CAMERA_H
#define CAMERA_H

#include <glm.hpp>

class Camera
{
private:
	glm::mat4 projection;
	glm::mat4 view;
	glm::vec3 position;
	glm::vec3 rotation;
public:
	void setProjection(glm::mat4 newProjection);
	void setView(glm::mat4 newView);
	void setPosition(glm::vec3 newPosition);
	void setRotation(glm::vec3 newRotation);
	glm::mat4 getProjection();
	glm::mat4 getView();
	glm::vec3 getPosition();
	glm::vec3 getRotation();
};

#endif