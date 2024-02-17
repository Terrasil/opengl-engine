#version 150 core

// zmienne wejsciowe
in vec4 inoutPos;
in vec3 inoutColor;
in vec2 inoutUV;

// uchwyt tekstury
uniform sampler2D tex;

// zmienna wyjsciowa
out vec4 outColor;


void main()
{   
	vec4 texColor = texture(tex, inoutUV);
	if(texColor.a == 0.0) discard;
	outColor = texColor;
}
