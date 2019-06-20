#version 300 es
precision mediump float;

layout(std140) uniform u_Matrices
{
	mat4 u_composedMatrix;
};

in vec3 a_position;

void main(void)
{
	gl_Position = u_composedMatrix * vec4(a_position, 1.0);
}
