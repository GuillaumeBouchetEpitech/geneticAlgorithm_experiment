
precision lowp float;

varying vec4 v_color;
varying vec3 v_normalInterp;
varying vec3 v_vertPos;

#include "assets/shaders/_common-apply-lighting.glsl.frag"

void main(void)
{
    gl_FragColor = vec4(applyLighting(v_color.xyz, v_normalInterp, v_vertPos), v_color.a);
}
