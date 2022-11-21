
precision highp float;

uniform mat4 u_composedMatrix;

attribute vec2 a_position;
attribute vec2 a_texCoord;
attribute vec3 a_offsetPosition;
attribute vec2 a_offsetTexCoord;
attribute vec3 a_offsetColor;
attribute float a_offsetScale;

varying vec2 v_texCoord;
varying vec3 v_color;

void main(void)
{
	vec2 position = a_position * a_offsetScale + a_offsetPosition.xy;

	gl_Position = u_composedMatrix * vec4(position, a_offsetPosition.z, 1.0);

	v_texCoord = a_texCoord + a_offsetTexCoord;
	v_color = a_offsetColor;
}
