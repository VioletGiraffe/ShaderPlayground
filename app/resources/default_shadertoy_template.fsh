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
vec3 iMouse = mousePosition;
float iGlobalTime = totalTime / 1000.0;
float iTimeDelta = frameTime;

%1%

void main()
{
	mainImage(gl_FragColor, pixelPosition);
}
