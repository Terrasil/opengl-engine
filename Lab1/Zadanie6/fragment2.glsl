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

	/*
	if (gl_PrimitiveID%2 == 0)
		outColor = vec4(1.0, 0.0, 0.0, 1.0);
	else
		outColor = vec4(0.0, 1.0, 0.0, 1.0);
	*/
	float x = (gl_FragCoord.x-250.0)/500.0;
	float y = (gl_FragCoord.y-250.0)/500.0;
	float root = 1.0-sqrt((x*x)+(y*y));
	outColor = vec4((root*root*root*root*root*(root)), 0.0, 0.0, 0.0);
}
