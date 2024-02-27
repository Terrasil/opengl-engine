#version 330 core

layout( location = 0 ) in vec4 inPosition;
layout( location = 2 ) in vec2 inUV;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;

uniform vec3 cameraPos;
out vec4 fragPos;
out vec4 realCameraPos;

void main()
{
	realCameraPos = vec4(cameraPos, 0.0);
	fragPos = inPosition;
	gl_Position = matProj * matView * matModel * inPosition;
}
