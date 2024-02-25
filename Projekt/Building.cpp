#include "Building.h"

void Building::draw(glm::vec3& cameraPos, glm::mat4& matProj, glm::mat4& matView) {
	glm::vec3 color = glm::vec3(0.0);
	model->draw(cameraPos, matProj, matView);
}
void Building::draw(Program* program, glm::vec3& cameraPos, glm::mat4& matProj, glm::mat4& matView) {
	glm::vec3 color = glm::vec3(0.0);
	model->draw(program, cameraPos, matProj, matView);
}
void Building::draw(glm::vec3& cameraPos, glm::mat4& matProj, glm::mat4& matView, glm::vec3& color) {
	model->draw(cameraPos, matProj, matView, color);
}