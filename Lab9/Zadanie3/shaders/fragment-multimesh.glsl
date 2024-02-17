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
uniform bool fogVisibility;
uniform bool showRefraction;
uniform bool showReflection;
uniform vec3 cameraPos;

uniform mat4 matProj;
uniform mat4 matView;
uniform mat4 matModel;

uniform samplerCube tex_skybox;

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

vec3 calculatePhongLight(vec3 inPosition, vec3 inNormal, Light light, Material material){
    vec3 normal = normalize(inNormal);
    vec3 viewDir = normalize(inPosition - fragPos.xyz);

    vec3 lightDir = normalize(light.Position - fragPos.xyz);
    float diff = max(dot(lightDir, normal), 0);
    vec3 diffusePart = diff * light.Diffuse * material.Diffuse;

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0), material.Shininess);
    vec3 specularPart = spec * light.Specular * material.Specular;

    float distanceToLight = distance(fragPos.xyz, light.Position);
    float attenuation = 10.0 / (light.Attenuation.x + light.Attenuation.y * distanceToLight + light.Attenuation.z * distanceToLight * distanceToLight);

    return (light.Ambient * material.Ambient) + (attenuation * (diffusePart + specularPart));
}

vec3 calculateBlinnPhongLight(vec3 inPosition, vec3 inNormal, Light light, Material material){
    vec3 normal = normalize(inNormal);
    vec3 viewDir = normalize(inPosition - fragPos.xyz);

    vec3 lightDir = normalize(light.Position - fragPos.xyz);
    float diff = max(dot(lightDir, normal), 0);
    vec3 diffusePart = diff * light.Diffuse * material.Diffuse;

    vec3 H = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, H), 0), material.Shininess);
    vec3 specularPart = spec * light.Specular * material.Specular;

    float LV = distance(fragPos.xyz, light.Position);
    float latt = 10.0 / (light.Attenuation.x + light.Attenuation.y * LV + light.Attenuation.z * LV * LV);

    vec3 ambientPart = light.Ambient * material.Ambient;
    vec3 lightCoef = ambientPart + latt * (diffusePart + specularPart);

    return lightCoef;
}
vec3 calculateDirectionalPhongLight(vec3 inPosition, vec3 inNormal, Light light, Material material){
    vec3 normal = normalize(inNormal);
    vec3 viewDir = normalize(inPosition - fragPos.xyz);
    
    vec3 lightDir = normalize(light.Position);
    float diff = max(dot(lightDir, normal), 0);
    vec3 diffusePart = diff * light.Diffuse * material.Diffuse;

    vec3 reflectDir = reflect(-lightDir, normal);
    float spec = pow(max(dot(viewDir, reflectDir), 0), material.Shininess);
    vec3 specularPart = spec * light.Specular * material.Specular;

    float distanceToLight = 0.0f;
    float attenuation = 10.0 / (light.Attenuation.x + light.Attenuation.y * distanceToLight + light.Attenuation.z * distanceToLight * distanceToLight);

    return (light.Ambient * material.Ambient) + (attenuation * (diffusePart + specularPart));
}

vec3 calculateDirectionalBlinnPhongLight(vec3 inPosition, vec3 inNormal, Light light, Material material){
    vec3 normal = normalize(inNormal);
    vec3 viewDir = normalize(inPosition - fragPos.xyz);
    
    vec3 lightDir = normalize(light.Position);
    float diff = max(dot(lightDir, normal), 0);
    vec3 diffusePart = diff * light.Diffuse * material.Diffuse;

    vec3 H = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, H), 0), material.Shininess);
    vec3 specularPart = spec * light.Specular * material.Specular;
    
    float LV = 0.0f;
    float latt = 10.0 / (light.Attenuation.x + light.Attenuation.y * LV + light.Attenuation.z * LV * LV);

    vec3 ambientPart = light.Ambient * material.Ambient;
    vec3 lightCoef = ambientPart + latt * (diffusePart + specularPart);

    return lightCoef;
}

vec3 calculateLight(Light light[32], Material material){
	vec3 lightValue = vec3(0.0);
	if(lightQuality){
		for (int i = 0; i < 32; ++i) {
			vec4 position = matModel * realCameraPos;
			if(!lightMode){
				lightValue = max(lightValue, calculatePhongLight(position.xyz, fragNormal, light[i], material));
			} else {
				lightValue = max(lightValue, calculateBlinnPhongLight(position.xyz, fragNormal, light[i], material));
			}
		} 
	} else {
		lightValue = lightCoef;
	}
	return lightValue;
}
vec3 calculateDirectionalLight(Light light[32], Material material){
	vec3 lightValue = vec3(0.0);
	if(lightQuality){
		for (int i = 0; i < 32; ++i) {
			vec4 position = matModel * realCameraPos;
			if(!lightMode){
				lightValue = max(lightValue, calculateDirectionalPhongLight(position.xyz, fragNormal, light[i], material));
			} else {
				lightValue = max(lightValue, calculateDirectionalBlinnPhongLight(position.xyz, fragNormal, light[i], material));
			}
		} 
	} else {
		lightValue = lightCoef;
	}
	return lightValue;
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

void main()
{
	vec4 finalColor;
	vec4 baseColor;

	if(useTexture){
		vec4 texColor = texture(tex, fragUV);
		baseColor = vec4(texColor.rgb, texColor.a);
	}else{
		baseColor = missingTexture();
	}

	if(baseColor.a <= 0.1) discard;

	vec3 Dir = normalize(fragPos.xyz - cameraPos.xyz);
	vec3 Refraction = refract(Dir, fragNormal, material.RefractionCoefficient);
	vec4 RefractionColor = texture(tex_skybox, Refraction);
	vec3 Reflection = reflect(Dir, fragNormal);
	vec4 ReflectionColor = texture(tex_skybox, Reflection);
	if (!showRefraction) {
		RefractionColor = baseColor;
	}
	if (!showReflection) {
		ReflectionColor = baseColor;
	}
	float weights = material.Refraction + material.Reflection + 1.0;
	finalColor = (RefractionColor * (material.Refraction/weights)) + (ReflectionColor * (material.Reflection/weights)) + (baseColor * (1.0/weights));
	finalColor.a = baseColor.a;

	if(lightVisibility){
		vec3 lightCoef = calculateDirectionalLight(dirLight, material);
		lightCoef = max(lightCoef, calculateLight(light, material));
		finalColor.rgb *= lightCoef;
	}

	if(fogVisibility){
		finalColor = calculateFog(finalColor);
	}


    outColor = finalColor;
}
