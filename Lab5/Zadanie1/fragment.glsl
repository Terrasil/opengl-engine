#version 150 core

// zmienne wejsciowe
in vec4 inoutPos;
in vec3 inoutColor;
in vec2 inoutUV;

// uchwyt tekstury
uniform sampler2D tex;
uniform sampler2D tex2;
uniform sampler2D mask;

// zmienna wyjsciowa
out vec4 outColor;


void main()
{
	// pobranie wartosci tekstury dla zadanej
	// wspolrzednej UV
	vec4 texColor = texture(tex, inoutUV);
	vec4 texColor2 = texture(tex2, inoutUV);
	vec4 mask = texture(mask, inoutUV);

	vec4 finalColor = mix(texColor,texColor2,mask.r);
	
	outColor = finalColor;
}
