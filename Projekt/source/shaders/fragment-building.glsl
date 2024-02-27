#version 330 core

out vec4 outColor;

in vec4 fragPos;
in vec3 fragNormal;
in vec2 fragUV;
in vec4 fragPosLight;
in vec3 lightCoef;
in vec4 realCameraPos;

uniform sampler2D tex;
uniform sampler2D shadowMap;
uniform int state;

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

float calcDirectionalShadow(vec4 fragPosLight, vec3 fragNormal, vec3 lightDirection)
{
	// Brak cienia
	// return 0;

	// Korekcja perspektywiczna (dla oswietlenia kierunkowego niepotrzebna)
    vec3 projCoords = fragPosLight.xyz / fragPosLight.w;
    // przeksztalcenie wartosci [-1,+1] na [0,1]
    projCoords = projCoords * 0.5 + 0.5;

    // pobranie z tekstury shadowMap odleglosci od zrodla swiatla fragmentu
    // do fragmentu oswietlonego na drodze do aktualnego fragmentu
    float closestDepth = texture(shadowMap, projCoords.xy).r;

    // obliczenie aktualnej odleglosci od zrodla swiatla
    float currentDepth = projCoords.z;

    // Sprawdzenie czy fragment jest w cieniu po odleglosci
	//return (currentDepth > closestDepth) ? 1.0 : 0.0;

	// Shadow acne
	// Parametry i stale ustawiamy eksperymentalnie
	float bias = max(0.001 * (1.0 - dot(fragNormal, normalize(lightDirection))), 0.001);
	//return (currentDepth - bias > closestDepth) ? 1.0 : 0.0;
	float shadow = 0.0;
	vec2 texelSize = 1.0 / textureSize(shadowMap, 0);
	for (float x = -2; x <= 2; x += 1.0) {
		for (float y = -2; y <= 2; y +=1.0) {
			float pcfDepth = texture(shadowMap, projCoords.xy + vec2 (x, y) * texelSize).r;
			shadow += currentDepth - bias > pcfDepth ? 1.0 : 0.0;
		}
	}
	return ( shadow / 25.0);
}

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
	vec3 fogColor = texture(tex_skybox, refract(normalize(fragPos.xyz - cameraPos.xyz), fragNormal, 1.0)).rgb;
	float fogStart = 75.0;
    float fogEnd = 100.0;
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
	
	if(state == 1){
		baseColor = vec4(0.0, baseColor.g, 0.0, baseColor.a);
	}else if(state == 2){
		baseColor = vec4(baseColor.r, 0.0, 0.0, baseColor.a);
	}

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
		float shadowPart = calcDirectionalShadow(fragPosLight, fragNormal, vec3(2.0, 3.0, 1.0));
		finalColor.rgb *= (lightCoef * (1 - shadowPart));
	}
	if(fogVisibility){
		finalColor = calculateFog(finalColor);
	}

    outColor = finalColor;
}
