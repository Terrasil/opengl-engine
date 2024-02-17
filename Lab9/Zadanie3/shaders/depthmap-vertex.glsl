#version 330 core
layout (location = 0) in vec4 inPosition;

uniform mat4 matProj;
uniform mat4 matView;

void main()
{
    gl_Position = matProj * matView * inPosition;
}
