#version 300 es

precision highp float;

uniform mat4 u_composedMatrix;

in vec2 a_position;
in vec2 a_texCoord;
in vec3 a_offsetPosition;
in vec2 a_offsetTexCoord;
in vec4 a_offsetColor;
in float a_offsetScale;

out vec2 v_texCoord;
out vec4 v_color;

void main(void)
{
	vec2 position = a_position * a_offsetScale + a_offsetPosition.xy;

	gl_Position = u_composedMatrix * vec4(position, a_offsetPosition.z, 1.0);

	v_texCoord = a_texCoord + a_offsetTexCoord;
	v_color = a_offsetColor;
}
