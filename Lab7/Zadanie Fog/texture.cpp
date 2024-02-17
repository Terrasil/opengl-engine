#include "texture.h"

#include "stb_image.h"

void Texture::load(std::string path, GLuint loadFormat) {
	data = stbi_load(path.c_str(), &width, &height, &comp, 0);
	if (data == NULL) {
		printf("Image cant be loaded!\n");
	}
	else {
		format = loadFormat;
		glGenTextures(1, &(id));
		glBindTexture(GL_TEXTURE_2D, id);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	}
}

GLuint Texture::getId() {
	return id;
}

int Texture::getWidth() {
	return width;
}

int Texture::getHeight() {
	return height;
}

int Texture::getFormat() {
	return format;
}