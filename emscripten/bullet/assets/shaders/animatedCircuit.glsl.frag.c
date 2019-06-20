#version 300 es
precision mediump float;

in vec4 v_color;

out vec4 o_color;

void main(void)
{
    o_color = v_color;
}
