#version 300 es

precision lowp float;

in vec3 v_color;

layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_outline;

void main(void)
{
  out_color = vec4(v_color, 1.0);
  out_outline = vec4(1.0);
}
