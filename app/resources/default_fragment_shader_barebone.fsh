#ifdef GL_ES
precision highp int;
precision highp float;
#endif

varying highp vec2 pixelPosition;
uniform mediump vec2 screenSize;
uniform mediump vec3 mousePosition;

uniform highp float frameTime;
uniform mediump float totalTime;

void main()
{
	vec2 uv = -1. + 2. * pixelPosition/screenSize;
	gl_FragColor = vec4(
		abs(tan(cos(totalTime/1000.0 + 3.0 * uv.y) * 2.0 * uv.x + totalTime/1000.0)),
		abs(tan(sin(totalTime/1000.0 + 2.0 * uv.x) * 3.0 * uv.y + totalTime/1000.0)),
		mousePosition.y/screenSize.y/2.0 + mousePosition.x/screenSize.x/2.0,
		1.0);
}
