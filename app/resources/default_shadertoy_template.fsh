#ifdef GL_ES
precision highp int;
precision highp float;
#endif

varying highp vec2 pixelPosition;
uniform mediump vec2 screenSize;
uniform mediump vec2 mousePosition;

uniform highp float frameTime;
uniform mediump float totalTime;

// ShaderToy compatibility variables
vec2 iResolution = screenSize;
float iGlobalTime = totalTime / 1000.0;

%1%

void main()
{
	mainImage(gl_FragColor, pixelPosition);
}
