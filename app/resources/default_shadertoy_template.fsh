#ifdef GL_ES
precision highp int;
precision highp float;
#endif

varying highp vec2 pixelPosition;
uniform mediump vec2 screenSize;
uniform mediump vec3 mousePosition;

uniform highp float frameTime;
uniform mediump float totalTime;

// ShaderToy compatibility variables
vec2 iResolution = screenSize;
vec3 iMouse = vec3(mousePosition.x, screenSize.y - mousePosition.y, mousePosition.z);
float iGlobalTime = totalTime / 1000.0;
float iTimeDelta = frameTime;
#define iTime iGlobalTime

%1%

void main()
{
	pixelPosition.y = screenSize.y - pixelPosition.y;
	mainImage(gl_FragColor, pixelPosition);
}
