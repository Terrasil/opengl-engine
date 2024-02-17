#version 150 core

// zmienne wejsciowe
in vec4 inoutPos;
in vec3 inoutColor;
in vec2 inoutUV;

// uchwyt tekstury
uniform sampler2D tex;
uniform int spriteVFrames;
uniform int spriteHFrames;
uniform int vFrame;
uniform int hFrame;

// zmienna wyjsciowa
out vec4 outColor;


void main()
{   
	vec2 frameSize = vec2(1.0 / float(spriteVFrames), 1.0 / float(spriteHFrames));
    vec2 uv = vec2(frameSize.x * float(vFrame), frameSize.y * float(hFrame)) + inoutUV * frameSize;
	vec4 texColor = texture(tex, uv);
	outColor = texColor;
}
