#version 330 core

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in float inPointSize;
out vec3 fragColor;

void main()
{
    gl_Position = inPosition;
	gl_PointSize = inPointSize;
    fragColor = inColor;
}
