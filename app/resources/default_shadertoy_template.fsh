#version 130
#ifdef GL_ES
precision highp int;
precision highp float;
#endif

out vec4 outColor4;

highp in vec2 pixelPosition;
uniform mediump vec2 iResolution;
uniform mediump vec4 mousePosition;

uniform highp float frameTime;
uniform mediump float totalTime;

uniform int iFrame;

uniform mediump vec4 iDate;

// ShaderToy compatibility variables
vec4 iMouse = vec4(mousePosition.x, iResolution.y - mousePosition.y, mousePosition.z, iResolution.y - mousePosition.w);
float iGlobalTime = totalTime / 1000.0;
#define iTimeDelta frameTime;
#define iTime iGlobalTime

%1%

void main()
{
	mainImage(outColor4, vec2(pixelPosition.x, iResolution.y - pixelPosition.y));
}
