#version 150 core

// zmienne wejsciowe
in vec4 inoutPos;
in vec3 inoutColor;
in vec2 inoutUV;

// uchwyt tekstury
uniform sampler2D tex;
uniform int time;

// zmienna wyjsciowa
out vec4 outColor;

float speed = 0.003;

void main()
{   
    vec2 uv = inoutUV;
	uv.x -= speed * time;
	vec4 texColor = texture(tex, uv);
	outColor = texColor;
}
