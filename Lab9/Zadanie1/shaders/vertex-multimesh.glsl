#version 330 core

layout( location = 0 ) in vec4 inPosition;
layout( location = 1 ) in vec3 inNormal;
layout( location = 2 ) in vec2 inUV;
layout( location = 3 ) in mat4 matModelInst;

uniform vec3 cameraPos;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;

uniform bool lightQuality;
uniform bool lightMode;
uniform bool lightVisibility;
uniform float time;

out vec4 fragPos;
out vec2 fragUV;
out vec3 fragNormal;

out vec3 lightCoef;

out vec4 realCameraPos;

struct Light {
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	vec3 Attenuation;
	vec3 Position;
};

uniform Light light[32];
uniform Light dirLight[32];

struct Material
{
	vec3 Ambient;
	vec3 Diffuse;
	vec3 Specular;
	float Shininess;
	float Refraction;
	float RefractionCoefficient;
	float Reflection;
};

uniform Material material;


vec3 calculatePhongLight(vec4 pos, vec3 norm, Light light, Material material){
    vec3 normal = normalize(norm);
    vec3 viewDir = normalize(cameraPos - pos.xyz);

    vec3 lightDir = normalize(light.Position - pos.xyz);
    float diff = max(dot(lightDir, normal), 0);
    vec3 diffusePart = diff * light.Diffuse * material.Diffuse;

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0), material.Shininess);
    vec3 specularPart = spec * light.Specular * material.Specular;

    float distanceToLight = distance(pos.xyz, light.Position);
    float attenuation = 10.0 / (light.Attenuation.x + light.Attenuation.y * distanceToLight + light.Attenuation.z * distanceToLight * distanceToLight);

    return (light.Ambient * material.Ambient) + (attenuation * (diffusePart + specularPart));
}

vec3 calculateBlinnPhongLight(vec4 pos, vec3 norm, Light light, Material material){
    vec3 viewDir = normalize(cameraPos - pos.xyz);
    vec3 normal = normalize(norm);

    vec3 lightDir = normalize(light.Position - pos.xyz);
    float diff = max(dot(lightDir, normal), 0);
    vec3 diffusePart = diff * light.Diffuse * material.Diffuse;

    vec3 H = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, H), 0), material.Shininess);
    vec3 specularPart = spec * light.Specular * material.Specular;

    float LV = distance(pos.xyz, light.Position);
    float latt = 10.0 / (light.Attenuation.x + light.Attenuation.y * LV + light.Attenuation.z * LV * LV);

    vec3 ambientPart = light.Ambient * material.Ambient;
    vec3 lightCoef = ambientPart + latt * (diffusePart + specularPart);

    return lightCoef;
}

vec3 calculateDirectionalPhongLight(vec4 pos, vec3 norm, Light light, Material material){
    vec3 normal = normalize(norm);
    vec3 viewDir = normalize(cameraPos - pos.xyz);

    vec3 lightDir = normalize(light.Position);
    float diff = max(dot(lightDir, normal), 0);
    vec3 diffusePart = diff * light.Diffuse * material.Diffuse;

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0), material.Shininess);
    vec3 specularPart = spec * light.Specular * material.Specular;

    float distanceToLight = 0.0;
    float attenuation = 10.0 / (light.Attenuation.x + light.Attenuation.y * distanceToLight + light.Attenuation.z * distanceToLight * distanceToLight);

    return (light.Ambient * material.Ambient) + (attenuation * (diffusePart + specularPart));
}

vec3 calculateDirectionalBlinnPhongLight(vec4 pos, vec3 norm, Light light, Material material){
    vec3 viewDir = normalize(cameraPos - pos.xyz);
    vec3 normal = normalize(norm);

    vec3 lightDir = normalize(light.Position);
    float diff = max(dot(lightDir, normal), 0);
    vec3 diffusePart = diff * light.Diffuse * material.Diffuse;

    vec3 H = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, H), 0), material.Shininess);
    vec3 specularPart = spec * light.Specular * material.Specular;

    float LV = 0.0;
    float latt = 10.0 / (light.Attenuation.x + light.Attenuation.y * LV + light.Attenuation.z * LV * LV);

    vec3 ambientPart = light.Ambient * material.Ambient;
    vec3 lightCoef = ambientPart + latt * (diffusePart + specularPart);

    return lightCoef;
}

vec3 calculateLight(Light light[32], Material material){
	vec3 lightValue = vec3(0.0);
	if(!lightQuality){
		for (int i = 0; i < 32; ++i) {
		    if(!lightMode){
			    lightValue = max(lightValue, calculatePhongLight(matModelInst * inPosition, fragNormal, light[i], material));
		    } else {
			    lightValue = max(lightValue, calculateBlinnPhongLight(matModelInst * inPosition, fragNormal, light[i], material));
		    }
	    } 
	}else {
		lightValue = lightCoef;
	}
	return lightValue;
}

vec3 calculateDirectionalLight(Light light[32], Material material){
	vec3 lightValue = vec3(0.0);
	if(!lightQuality){
		for (int i = 0; i < 32; ++i) {
		    if(!lightMode){
			    lightValue = max(lightValue, calculateDirectionalPhongLight(matModelInst * inPosition, fragNormal, light[i], material));
		    } else {
			    lightValue = max(lightValue, calculateDirectionalBlinnPhongLight(matModelInst * inPosition, fragNormal, light[i], material));
		    }
	    }
    } else {
		lightValue = lightCoef;
	}
	return lightValue;
}

void main()
{
    vec4 finalPosition = inPosition;
    finalPosition.y -= time;
	fragPos = matModelInst * finalPosition;
	fragUV = inUV;
	fragNormal = mat3(transpose(inverse(matModelInst))) * inNormal;

	realCameraPos = vec4(cameraPos, 0.0);

	lightCoef = vec3(0.0);
	
	if(lightVisibility){
		lightCoef = calculateDirectionalLight(dirLight, material);
		lightCoef = max(lightCoef, calculateLight(light, material));
	}

	gl_Position = matProj * matView * matModelInst * finalPosition;
}
