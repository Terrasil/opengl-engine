#version 330 core

in vec3 fragColor;

out vec4 outColor;

void main()
{
	float gray = dot(fragColor, vec3(1.0));
    outColor = vec4(vec3(gray), 1.0);
}