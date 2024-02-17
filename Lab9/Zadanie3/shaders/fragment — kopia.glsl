#version 330 core

out vec4 outColor;

in vec4 fragPos;
in vec3 fragNormal;
in vec2 fragUV;
in vec3 lightCoef;
in vec4 realCameraPos;

uniform sampler2D tex;
uniform bool useTexture;
uniform bool lightQuality;
uniform bool lightMode;
uniform bool lightVisibility;
uniform vec3 cameraPosition;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;

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


struct Material
{
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	float Shininess;
};

uniform Material material;

vec3 calculatePhongLight(LightParam light, Material material){
    vec3 normal = normalize(fragNormal);
    vec3 viewDir = normalize(cameraPosition - fragPos.xyz);

    vec3 lightDir = normalize(light.Position - fragPos.xyz);
    float diff = max(dot(lightDir, normal), 0);
    vec3 diffusePart = diff * light.Diffuse * material.Diffuse;

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0), material.Shininess);
    vec3 specularPart = spec * light.Specular * material.Specular;

    float distanceToLight = distance(fragPos.xyz, light.Position);
    float attenuation = 10.0 / (light.Att.x + light.Att.y * distanceToLight + light.Att.z * distanceToLight * distanceToLight);

    return (light.Ambient * material.Ambient) + (attenuation * (diffusePart + specularPart));
}

vec3 calculateBlinnPhongLight(LightParam light, Material material){
    vec3 viewDir = normalize(cameraPosition - fragPos.xyz);
    vec3 normal = normalize(fragNormal);

    vec3 lightDir = normalize(light.Position - fragPos.xyz);
    float diff = max(dot(lightDir, normal), 0);
    vec3 diffusePart = diff * light.Diffuse * material.Diffuse;

    vec3 H = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, H), 0), material.Shininess);
    vec3 specularPart = spec * light.Specular * material.Specular;

    float LV = distance(fragPos.xyz, light.Position);
    float latt = 10.0 / (light.Att.x + light.Att.y * LV + light.Att.z * LV * LV);

    vec3 ambientPart = light.Ambient * material.Ambient;
    vec3 lightCoef = ambientPart + latt * (diffusePart + specularPart);

    return lightCoef;
}

vec3 calculateLight(LightParam light, Material material){
	vec3 lightValue = vec3(0.0);
	if(lightQuality){
		if(!lightMode){
			lightValue = max(lightValue, calculatePhongLight(light, material));
		} else {
			lightValue = max(lightValue, calculateBlinnPhongLight(light, material));
		}
	} else {
		lightValue = lightCoef;
	}
	return lightValue;
}

vec4 calculateFog(vec4 baseColor){
	float distanceToCamera = length(realCameraPos.xyz - fragPos.xyz);
    vec3 fogColor = vec3(0.3, 0.3, 0.3);
	float fogStart = 25.0;
    float fogEnd = 50.0;
    float fogDensity = 1.0;

    float fogFactor = clamp((distanceToCamera - fogStart) / (fogEnd - fogStart), 0.0, 1.0);
    baseColor.rgb = mix(baseColor.rgb, fogColor, fogFactor * fogDensity);

	return baseColor;
	
}


vec4 missingTexture(){
	int gridSize = 16;
	int x = int(fragUV.x * gridSize);
	int y = int(fragUV.y * gridSize);

	if ((x + y) % 2 == 0) {
		return vec4(1.0, 0.0, 1.0, 1.0); // Pink color
	} else {
		return vec4(0.0, 0.0, 0.0, 1.0); // Black color
	}
}

void main()
{
	vec4 finalColor;

	if(useTexture){
		vec4 texColor = texture(tex, fragUV);
		finalColor = vec4(texColor.rgb, texColor.a);
	}else{
		finalColor = missingTexture();
	}

	if(lightVisibility){
		finalColor.rgb *= calculateLight(lightParam, material);
	}

    finalColor = calculateFog(finalColor);

    outColor = finalColor;
}
