
precision mediump float;

uniform mat4 u_composedMatrix;

attribute vec3 a_position;

void main(void)
{
	gl_Position = u_composedMatrix * vec4(a_position, 1.0);
}
