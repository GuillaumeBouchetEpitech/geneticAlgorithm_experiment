#version 300 es
precision mediump float;

in vec3 v_color;

out vec4 o_color;

void main(void)
{
	o_color = vec4(v_color, 1.0);
}
