#version 330 core

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec3 inNormal;
layout( location = 2 ) in vec2 inUV;
uniform vec3 cameraPos;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;

out vec4 fragPos;
out vec2 fragUV;
out vec3 fragNormal;
out vec3 lightCoef;

struct LightParam {
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	vec3 Position;
	vec3 Att;
};

LightParam lightParam = LightParam(
	vec3(0.1, 0.1, 0.1),
	vec3(1.0, 1.0, 1.0),
	vec3(1.0, 1.0, 1.0),
	vec3(2.0, 3.0, 1.0),
	vec3(1.0, 0.0, 1.0)
);

struct MaterialParam {
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	float Shine;
};

MaterialParam materialParam = MaterialParam(
	vec3(0.2, 0.2, 0.2),
	vec3(1.0, 1.0, 1.0),
	vec3(2.0, 2.0, 2.0),
	8.0
);

vec3 calculateLight(LightParam light, MaterialParam material){
	vec3 ambientPart = light.Ambient * material.Ambient;

	vec3 L = normalize(light.Position - inPosition.xyz);
	float diff = max(dot(L, inNormal), 0);
	vec3 diffusePart = diff * light.Diffuse * material.Diffuse;

	vec3 E = normalize(cameraPos - inPosition.xyz);
	vec3 R = reflect(-E, inNormal);
	float spec = pow(max(dot(R, L), 0), material.Shine);
	vec3 specularPart = spec  * light.Specular * material.Specular;

	float LV = distance(inPosition.xyz, light.Position);

	float latt =  10.0/(light.Att.x+light.Att.y * LV + light.Att.z*LV*LV);

	vec3 lightCoef = ambientPart + latt * (diffusePart+specularPart);

	return lightCoef;
};

void main()
{
	fragPos = inPosition;
	fragUV = inUV;
	fragNormal = inNormal;
	
	lightCoef = calculateLight(lightParam, materialParam);

	gl_Position = matProj * matView * matModel * inPosition;
}
