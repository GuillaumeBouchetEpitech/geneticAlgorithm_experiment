
precision highp float;

uniform mat4 u_composedMatrix;

attribute vec3 a_position;
attribute vec4 a_color;

varying vec4 v_color;

void main(void)
{
	gl_Position = u_composedMatrix * vec4(a_position, 1.0);

	v_color = a_color;
}
