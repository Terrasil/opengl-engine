// Vertex shader
#version 330 core

layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec3 inColor;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;

out vec3 fragColor;

void main()
{
	fragColor = inColor;
	vec4 finalPosition = matProj * matView * matModel * inPosition;
	gl_Position = finalPosition;
}
