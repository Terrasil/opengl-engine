#include "skybox.h"

#include <gtc/type_ptr.hpp>

#include <glew.h>
#include <freeglut.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

void SkyBox::setTopTexture(std::string topTexturePath) {
	texturePaths[GL_TEXTURE_CUBE_MAP_POSITIVE_Y] = topTexturePath;
}

void SkyBox::setBottomTexture(std::string bottomTexturePath) {
	texturePaths[GL_TEXTURE_CUBE_MAP_NEGATIVE_Y] = bottomTexturePath;
}

void SkyBox::setFrontTexture(std::string frontTexturePath) {
	texturePaths[GL_TEXTURE_CUBE_MAP_POSITIVE_Z] = frontTexturePath;
}

void SkyBox::setBackTexture(std::string backTexturePath) {
	texturePaths[GL_TEXTURE_CUBE_MAP_NEGATIVE_Z] = backTexturePath;
}

void SkyBox::setLeftTexture(std::string leftTexturePath) {
	texturePaths[GL_TEXTURE_CUBE_MAP_NEGATIVE_X] = leftTexturePath;
}

void SkyBox::setRightTexture(std::string rightTexturePath) {
	texturePaths[GL_TEXTURE_CUBE_MAP_POSITIVE_X] = rightTexturePath;
}

void SkyBox::setProgram(Program* newProgram) {
	program = newProgram;
}

void SkyBox::init() {
	GLuint programId = program->getId();

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	GLuint vBuffer_pos;
	glGenBuffers(1, &vBuffer_pos);
	glBindBuffer(GL_ARRAY_BUFFER, vBuffer_pos);
	glBufferData(GL_ARRAY_BUFFER, 24 * sizeof(GLfloat), positions, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, NULL);
	glEnableVertexAttribArray(0);

	GLuint vBuffer_idx;
	glGenBuffers(1, &vBuffer_idx);
	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, vBuffer_idx);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, 36 * sizeof(GLuint), indices, GL_STATIC_DRAW);

	glBindVertexArray(0);

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

	stbi_set_flip_vertically_on_load(false);

	for (auto texturePath : texturePaths) {
		int tex_width, tex_height, n;
		unsigned char* tex_data;
		tex_data = stbi_load(texturePath.second.c_str(), &tex_width, &tex_height, &n, 0);
		if (tex_data == NULL){
			printf("Image %s can't be loaded!\n", texturePath.second.c_str());
			exit(1);
		}
		glTexImage2D(texturePath.first, 0, GL_RGB, tex_width, tex_height, 0, GL_RGB, GL_UNSIGNED_BYTE, tex_data);
		stbi_image_free(tex_data);
	}
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_REPEAT);
	glGenerateMipmap(GL_TEXTURE_CUBE_MAP);

	stbi_set_flip_vertically_on_load(true);
}

void SkyBox::draw(glm::mat4 matProj, glm::mat4 matView) {
	GLuint programId = program->getId();
	glUseProgram(programId);

	glm::mat4 matPVM = matProj * matView * glm::scale(glm::mat4(1.0), glm::vec3(50.0, 50.0, 50.0));
	glUniformMatrix4fv(glGetUniformLocation(programId, "matPVM"), 1, GL_FALSE, glm::value_ptr(matPVM));

	glActiveTexture(GL_TEXTURE0);
	glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
	glUniform1i(glGetUniformLocation(programId, "tex_skybox"), 0);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, NULL);

	glUseProgram(0);
}