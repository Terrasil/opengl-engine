// Fragment shader
#version 330 core


// Predefiniowane dane wejsciowe
/*
in  vec4 gl_FragCoord;
in  int  gl_PrimitiveID;
*/
// Dane wyjsciowe
out vec4 outColor;
uniform float time;


void main()
{
    float red = 0.5 + 0.5 * sin(time);
    float green = 0.5 + 0.5 * sin(time + 2.0);
    float blue = 0.5 + 0.5 * sin(time + 4.0);
    outColor = vec4(red, green, blue, 1.0);
}
