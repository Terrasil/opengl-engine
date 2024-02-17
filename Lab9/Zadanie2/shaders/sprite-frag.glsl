#version 150 core

// zmienne wejsciowe
in vec4 inoutPos;
in vec3 inoutColor;
in vec2 inoutUV;

// uchwyt tekstury
uniform sampler2D tex;

// zmienna wyjsciowa
out vec4 outColor;

// Parametry obramowania
float radius = 0.49;
vec4 borderColor = vec4(0.0);
float borderThickness = 0.01;
vec4 color = vec4(0.0, 0.0, 0.0, 1.0);

void main()
{
	// pobranie wartosci tekstury dla zadanej
	// wspolrzednej UV
    float d = sqrt(dot(inoutUV - 0.5, inoutUV - 0.5));
	vec4 minimap;
	vec4 texColor = texture(tex, inoutUV);
	float t1 = 1.0 - smoothstep(radius - borderThickness, radius, d);
	float t2 = 1.0 - smoothstep(radius, radius + borderThickness, d);
	minimap = vec4(mix(color.rgb, texColor.rgb, t1), t2);
	outColor = minimap;
}
