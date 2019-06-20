#version 300 es
precision mediump float;

layout(std140) uniform u_Matrices
{
	mat4 u_composedMatrix;
};

in vec2 a_position;
in vec2 a_texCoord;
in vec2 a_offsetPosition;
in vec2 a_offsetTexCoord;
in float a_offsetScale;

out vec2 v_texCoord;

void main(void)
{
	vec2 position = a_position * a_offsetScale + a_offsetPosition;

	gl_Position = u_composedMatrix * vec4(position, 0.0, 1.0);
	v_texCoord = a_texCoord + a_offsetTexCoord;
}
