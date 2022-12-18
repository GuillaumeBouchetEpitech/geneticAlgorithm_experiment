#version 300 es

precision lowp float;

uniform sampler2D u_texture;

in vec2 v_texCoord;
in vec3 v_normal;
in vec3 v_vertPos;

layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_outline;

#include "assets/shaders/scene/_common-apply-lighting.glsl.frag"

void main(void)
{
  vec4 texColor = texture(u_texture, v_texCoord);
  out_color = vec4(applyLighting(texColor.xyz, v_normal, v_vertPos), texColor.a);
  out_outline = vec4(0.0);
}
