
precision mediump float;

uniform mat4 u_composedMatrix;

attribute vec3  a_position;
attribute vec3  a_offsetPosition;
attribute float a_offsetScale;
attribute vec4  a_offsetColor;

varying vec4 v_color;

void main(void)
{
	vec3 position = a_position * a_offsetScale + a_offsetPosition;

	gl_Position = u_composedMatrix * vec4(position, 1.0);

	v_color = a_offsetColor;
}
