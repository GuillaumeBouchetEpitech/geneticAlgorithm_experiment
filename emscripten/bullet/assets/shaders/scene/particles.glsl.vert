#version 300 es

precision highp float;

uniform mat4 u_composedMatrix;

in vec3  a_position;
in vec3  a_offsetPosition;
in float a_offsetScale;
in vec3  a_offsetColor;

out vec4 v_color;

void main(void)
{
	vec3 position = a_position * a_offsetScale + a_offsetPosition;

	gl_Position = u_composedMatrix * vec4(position, 1.0);

	v_color = vec4(a_offsetColor, 1.0);
}
