#version 330 core

out vec4 outColor;

in vec2 fragUV;
uniform vec3 color;
uniform bool fogVisibility;

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
	vec4 finalColor = vec4(color, 1.0);
	
	if(fogVisibility){
		finalColor = calculateFog(finalColor);
	}
	
    outColor = finalColor;
}
