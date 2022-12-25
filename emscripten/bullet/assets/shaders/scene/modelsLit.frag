#version 300 es

precision lowp float;

in vec4 v_color;
in vec4 v_outlineColor;
in vec3 v_normal;
in vec3 v_vertPos;

layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_outline;

#include "assets/shaders/scene/_common-apply-lighting.glsl.frag"

void main(void)
{
  out_color = vec4(applyLighting(v_color.rgb, v_normal, v_vertPos), v_color.a);
  out_outline = v_outlineColor;
}
