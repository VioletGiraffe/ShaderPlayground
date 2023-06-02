#version 130
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
	gl_FragColor = vec4(0.2);
}
