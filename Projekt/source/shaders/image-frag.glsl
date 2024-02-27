#version 150 core

// zmienne wejsciowe
in vec4 inoutPos;
in vec3 inoutColor;
in vec2 inoutUV;

// uchwyt tekstury
uniform sampler2D tex;
uniform bool flipX;
uniform bool flipY;

// zmienna wyjsciowa
out vec4 outColor;

// Parametry obramowania
float radius = 0.49;
vec4 borderColor = vec4(0.0);
float borderThickness = 0.01;
vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

void main()
{
	vec4 texColor = texture(tex, inoutUV);
	outColor = texColor;
}
