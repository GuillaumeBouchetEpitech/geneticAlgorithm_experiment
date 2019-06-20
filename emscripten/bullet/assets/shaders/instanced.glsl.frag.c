#version 300 es
precision mediump float;

layout(std140) uniform u_Colors
{
	vec4 u_color;
};

out vec4 o_color;

void main(void)
{
	o_color = u_color;
}
