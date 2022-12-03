
precision lowp float;

uniform sampler2D u_texture;

varying vec2 v_texCoord;
varying vec3 v_normalInterp;
varying vec3 v_vertPos;

#include "assets/shaders/_common-apply-lighting.glsl.frag"

void main(void)
{
  vec4 color = texture2D(u_texture, v_texCoord);
  gl_FragColor = vec4(applyLighting(color.xyz, v_normalInterp, v_vertPos), color.a);
}
