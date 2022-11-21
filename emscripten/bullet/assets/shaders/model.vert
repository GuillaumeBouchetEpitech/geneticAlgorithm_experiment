
precision highp float;

uniform mat4 u_composedMatrix;

attribute vec3 a_position;
attribute vec3 a_color;
attribute mat4 a_offsetTransform;
attribute vec3 a_offsetColor;

varying vec3 v_color;

void main(void)
{
	v_color = a_color * a_offsetColor;

	gl_Position = u_composedMatrix * a_offsetTransform * vec4(a_position, 1.0);
}
