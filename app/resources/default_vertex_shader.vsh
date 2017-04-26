#ifdef GL_ES
precision highp int;
precision highp float;
#endif

attribute highp vec4 vertexPosition;
uniform highp mat4 matrix;

varying highp vec2 pixelPosition;

void main()
{
	gl_Position = matrix * vertexPosition;
	pixelPosition = vertexPosition.xy;
}
