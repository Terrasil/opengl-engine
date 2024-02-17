#version 330 core

layout( location = 0 ) in vec4 inPosition;
layout( location = 2 ) in vec2 inUV;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;
out vec4 realCameraPos;
out vec4 fragPos;
uniform vec3 cameraPos;

out vec2 fragUV;

void main()
{
	fragPos = matModel * inPosition;
	realCameraPos = vec4(cameraPos, 0.0);
	fragUV = inUV;
	gl_Position = matProj * matView * matModel * inPosition;
}
