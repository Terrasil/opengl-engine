#version 330 core

layout( location = 0 ) in vec4 inPosition;
layout( location = 2 ) in vec2 inUV;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;

out vec2 fragUV;

void main()
{
	fragUV = inUV;
	gl_Position = matProj * matView * matModel * inPosition;
}
