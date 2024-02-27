#version 330 core

// Wspolrzedne wierzcholkow
layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec3 inColor;
layout( location = 2 ) in vec2 inUV;

// Macierz rzutowania

out vec4 inoutPos;
out vec3 inoutColor;
out vec2 inoutUV;

void main()
{
	inoutPos = inPosition;
	inoutColor = inColor;
	inoutUV = inUV;
	gl_Position = inPosition;

}
