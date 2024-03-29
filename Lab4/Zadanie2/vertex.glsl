// Vertex shader
#version 330

// Dane pobrane z VAO
layout (location = 0) in vec4 inPosition;
layout (location = 1) in vec3 inColor;

out vec3 fragColor;

uniform float AngleX;
uniform float AngleY;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;


void main()
{
	fragColor = inColor;

	vec4 finalPosition = matProj * matView * matModel * inPosition;

	gl_Position = finalPosition;
}
