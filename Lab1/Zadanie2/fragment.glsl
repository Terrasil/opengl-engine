#version 330 core

// Predefined input data
/*
in vec4 gl_FragCoord;
in int gl_PrimitiveID;
*/
// Output data
out vec4 outColor;

void main()
{
    int triangleID = gl_PrimitiveID % 3; // Assuming triangles are drawn in sets of 3

    if (triangleID == 0)
        outColor = vec4(1.0, 0.0, 0.0, 1.0); // Red
    else if (triangleID == 1)
        outColor = vec4(0.0, 1.0, 0.0, 1.0); // Green
    else
        outColor = vec4(0.0, 0.0, 1.0, 1.0); // Blue
}