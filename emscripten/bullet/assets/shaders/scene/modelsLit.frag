#version 300 es

precision lowp float;

in vec3 v_color;
in vec3 v_normal;
in vec3 v_vertPos;

layout(location = 0) out vec4 out_color;
layout(location = 1) out vec4 out_outline;

#include "assets/shaders/scene/_common-apply-lighting.glsl.frag"

void main(void)
{
  out_color = vec4(applyLighting(v_color, v_normal, v_vertPos), 1.0);
  out_outline = vec4(1.0);
}
