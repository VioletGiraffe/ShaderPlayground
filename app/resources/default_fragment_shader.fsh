#ifdef GL_ES
precision highp int;
precision highp float;
#endif

varying highp vec2 pixelPosition;

void main()
{
	gl_FragColor = pixelPosition.x > 100.0 ? vec4(1.0, 0.95, 0.0, 1.0) : vec4(0.0, 0.85, 1.0, 1.0);
}
