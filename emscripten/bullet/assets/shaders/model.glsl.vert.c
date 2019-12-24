
precision mediump float;

uniform mat4 u_composedMatrix;

attribute vec3 a_position;
attribute vec3 a_color;
attribute mat4 a_transform;

varying vec3 v_color;

void main(void)
{
	gl_Position = u_composedMatrix * a_transform * vec4(a_position, 1.0);

	v_color = a_color;
}
