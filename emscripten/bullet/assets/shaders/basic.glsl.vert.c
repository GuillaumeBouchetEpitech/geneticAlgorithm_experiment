#version 300 es
precision mediump float;

layout(std140) uniform u_Matrices
{
	mat4 u_composedMatrix;
};

in vec3 a_position;
in vec3 a_color;

out vec3 v_color;

void main(void)
{
	gl_Position = u_composedMatrix * vec4(a_position, 1.0);
	v_color = a_color;
}
