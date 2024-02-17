// Vertex shader
#version 330 core

// Dane pobrane z VAO
in vec4 inPosition;


// Predefiniowane dane wejsciowe
// nie odkomentowujemy!
/*
in int gl_VertexID;
in int gl_InstanceID;
*/

// Dane wyjsciowe (kolejny etap potoku)
// out vec4 gl_Position;
uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;



void main()
{

	// zmienna typu vec4
	vec4 finalPosition = matProj * matView * matModel * inPosition;


	// finalna pozycja wierzcholka
	gl_Position = finalPosition;
}
