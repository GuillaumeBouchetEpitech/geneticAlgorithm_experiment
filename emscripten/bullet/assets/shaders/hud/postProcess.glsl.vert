#version 300 es

precision highp float;

uniform mat4 u_composedMatrix;

in vec3 a_position;
in vec2 a_texCoord;

out vec2 v_texCoord;

void main(void)
{
	gl_Position = u_composedMatrix * vec4(a_position, 1.0);
	v_texCoord = a_texCoord;
}
