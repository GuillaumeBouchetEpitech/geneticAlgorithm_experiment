
precision mediump float;

uniform mat4 u_composedMatrix;

attribute vec2 a_position;
attribute vec2 a_texCoord;
attribute vec2 a_offsetPosition;
attribute vec2 a_offsetTexCoord;
attribute float a_offsetScale;

varying vec2 v_texCoord;

void main(void)
{
	vec2 position = a_position * a_offsetScale + a_offsetPosition;

	gl_Position = u_composedMatrix * vec4(position, 0.0, 1.0);
	v_texCoord = a_texCoord + a_offsetTexCoord;
}
