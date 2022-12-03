precision highp float;

uniform mat4 u_composedMatrix;
uniform float u_animationCoef;

attribute vec3 a_position;
attribute vec3 a_animationNormal;
attribute vec2 a_texCoord;

varying vec2 v_texCoord;

void main(void)
{
	gl_Position = u_composedMatrix * vec4(a_position + a_animationNormal * u_animationCoef, 1.0);

	v_texCoord = a_texCoord;
}
