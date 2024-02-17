#include "program.h"

void Program::init() {
	id = glCreateProgram();
}

GLuint Program::getId() {
	return id;
}