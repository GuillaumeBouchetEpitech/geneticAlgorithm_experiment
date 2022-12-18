#version 300 es

precision lowp float;

uniform sampler2D u_texture;

in vec2 v_texCoord;

layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_outline;

void main(void)
{
  out_color = texture(u_texture, v_texCoord);
  out_outline = vec4(0.0);
}
