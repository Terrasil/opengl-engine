// Vertex shader
#version 330 core

layout (location = 0) in vec4 inPosition;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;

void main()
{
	vec4 finalPosition = matProj * matView * matModel * inPosition;
	gl_Position = finalPosition;
}
