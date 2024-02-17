#version 330 core

layout(location = 0) in vec4 inPosition;
layout(location = 1) in vec3 inColor;
out vec3 fragColor;

void main()
{
    gl_Position = inPosition;
    fragColor = inColor;
}
