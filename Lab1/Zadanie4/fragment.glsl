// Fragment shader
#version 330 core


// Predefiniowane dane wejsciowe
/*
in  vec4 gl_FragCoord;
in  int  gl_PrimitiveID;
*/
// Dane wyjsciowe
out vec4 outColor;


void main()
{
	outColor = vec4(gl_PrimitiveID/100.0, 0.0, 0.0, 0.0);
}
