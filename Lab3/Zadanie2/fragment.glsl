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

	
	if (gl_PrimitiveID%3 == 0)
		outColor = vec4(gl_PrimitiveID/9.0, 0.0, 0.0, 1.0);
	else if(gl_PrimitiveID%3 == 1)
		outColor = vec4(0.0, gl_PrimitiveID/9.0, 0.0, 1.0);
	else
		outColor = vec4(0.0, 0.0, gl_PrimitiveID/9.0, 1.0);

}
