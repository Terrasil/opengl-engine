// Fragment shader
#version 330 core


// Predefiniowane dane wejsciowe
/*
in  vec4 gl_FragCoord;
in  int  gl_PrimitiveID;
*/
uniform vec2 screenSize;
// Dane wyjsciowe
out vec4 outColor;


void main()
{
	float x = (gl_FragCoord.x-(screenSize.x/2.0))/screenSize.x;
	float y = (gl_FragCoord.y-(screenSize.x/2.0))/screenSize.x;
	float root = 1.0-sqrt((x*x)+(y*y));
	outColor = vec4((root*root*root*root*root*(root)), 0.0, 0.0, 0.0);
}
