#version 130
#ifdef GL_ES
precision highp int;
precision highp float;
#endif

in vec3 vertexPosition;
uniform highp mat4 matrix;

highp out vec2 pixelPosition;

void main()
{
	gl_Position = matrix * vec4(vertexPosition, 1.0f);
	pixelPosition = vertexPosition.xy;
}
