#version 300 es

precision lowp float;

uniform vec4 u_color;

layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_outline;

void main(void)
{
	out_color = u_color;
  out_outline = vec4(0.0);
}
