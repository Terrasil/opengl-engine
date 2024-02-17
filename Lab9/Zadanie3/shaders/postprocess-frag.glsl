#version 150 core

// zmienne wejsciowe
in vec4 inoutPos;
in vec3 inoutColor;
in vec2 inoutUV;

// uchwyt tekstury
uniform sampler2D tex;
uniform int style;
uniform float offsetX;
uniform float offsetY;

// zmienna wyjsciowa
out vec4 outColor;

const float vignette_intensity = 0.4;
const float vignette_opacity = 0.5;

vec4 kernel(float kernel[9]){
	vec2 offsets[9] = vec2[](
        vec2(-offsetX,  offsetY), // top-left
        vec2( 0.0f,     offsetY), // top-center
        vec2( offsetX,  offsetY), // top-right
        vec2(-offsetX,  0.0f),   // center-left
        vec2( 0.0f,     0.0f),   // center-center
        vec2( offsetX,  0.0f),   // center-right
        vec2(-offsetX, -offsetY), // bottom-left
        vec2( 0.0f,    -offsetY), // bottom-center
        vec2( offsetX, -offsetY)  // bottom-right    
    );
	vec3 sampleTex[9];
    for(int i = 0; i < 9; i++)
    {
        sampleTex[i] = vec3(texture(tex, inoutUV + offsets[i]));
    }
    vec3 col = vec3(0.0);
    for(int i = 0; i < 9; i++)
        col += sampleTex[i] * kernel[i];
    
    return vec4(col, 1.0);
}
float vignette(vec2 uv){
	uv *= 1.0 - uv.xy;
	float vignette = uv.x * uv.y * 15.0;
	return pow(vignette, vignette_intensity * vignette_opacity);
}

void main()
{
	vec4 texColor = texture(tex, inoutUV);
	if(style==0) // standard
    {
        outColor = texColor;
    }
    else if(style==1) // grayscale
    {
        float grayscale = (texColor.x+texColor.y + texColor.z)/3;
        outColor = vec4(grayscale, grayscale, grayscale, texColor.a);
    }
	else if(style==2) // negative
    {
		vec3 negColor = vec3(1.0) - texColor.rgb;
        outColor = vec4(negColor, texColor.a);
    }
	else if(style==3) // sharpening
    {
		outColor = kernel(float[](
			2.0,  2.0, 2.0,
			2.0,-15.0, 2.0,
			2.0,  2.0, 2.0
		));
    }
	else if(style==4) // blur
    {
		outColor = kernel(float[](
			1.0 / 16, 2.0 / 16, 1.0 / 16,
			2.0 / 16, 4.0 / 16, 2.0 / 16,
			1.0 / 16, 2.0 / 16, 1.0 / 16
		));
    }
	else if(style==5) // edge
    {
        outColor = kernel(float[](
			1.0, 1.0, 1.0,
			1.0,-8.0, 1.0,
			1.0, 1.0, 1.0
		));
    }
	else if(style==6) // vignette 
    {
        //vec2 uv = inoutUV.xy / vec2(1.0 / offsetX, 1.0 / offsetY);
		//uv *=  1.0 - uv.yx;
		//float vig = uv.x*uv.y * 5000.0;
		//vig = pow(vig, 0.25);

		outColor = mix(vec4(0.0,0.0,0.0,1.0), texColor, vignette(inoutUV)); 
    }
	else // error
    {
        outColor = vec4(1.0, 0.0, 1.0, 1.0);
    }
}
