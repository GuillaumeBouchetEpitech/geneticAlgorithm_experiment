
precision mediump float;

uniform mat4 u_composedMatrix;

attribute vec3 a_position;
attribute vec2 a_texCoord;

varying vec2 v_texCoord;

void main(void)
{
	gl_Position = u_composedMatrix * vec4(a_position, 1.0);

	v_texCoord = a_texCoord;
}
