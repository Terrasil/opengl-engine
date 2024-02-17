#version 150 core

out vec4 outColor;

in vec4 fragPos;
in vec3 fragNormal;
in vec2 fragUV;
in vec3 lightCoef;

uniform sampler2D tex;
uniform bool useTexture;
uniform bool hdLight;
uniform vec3 cameraPosition;

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

	vec3 L = normalize(light.Position - fragPos.xyz);
	float diff = max(dot(L, fragNormal), 0);
	vec3 diffusePart = diff * light.Diffuse * material.Diffuse;

	vec3 E = normalize(cameraPosition - fragPos.xyz);
	vec3 R = reflect(-E, fragNormal);
	float spec = pow(max(dot(R, L), 0), material.Shine);
	vec3 specularPart = spec  * light.Specular * material.Specular;

	float LV = distance(fragPos.xyz, light.Position);

	float latt =  10.0/(light.Att.x+light.Att.y * LV + light.Att.z*LV*LV);

	vec3 lc = ambientPart + latt * (diffusePart+specularPart);

	return lc;
};

void main()
{
	vec3 light = lightCoef;
	if(hdLight){
		light = calculateLight(lightParam, materialParam);
	}
	if(useTexture){
		vec4 texColor = texture(tex, fragUV);
		outColor = vec4(light * texColor.rgb, texColor.a);
	}else{
		float dist = length(fragPos);
		vec3 crazyColor = vec3(
			sin(dist*3.0)/2.0 + 0.5,
			sin(fragPos.y)/2.0 + 0.5,
			0.5
		);
		vec3 fragColor = light * crazyColor;
		outColor = vec4(fragColor, 1.0);
	}
}
