#version 300 es

precision lowp float;

uniform sampler2D u_texture;

in vec2 v_texCoord;
in vec4 v_color;

out vec4 out_color;

void main(void)
{
    vec4 texColor = texture(u_texture, v_texCoord);
    if (texColor.a < 0.5)
    {
        discard;
    }
    else
    {
        out_color = vec4(texColor.xyz * v_color.xyz, v_color.a);
    }
}
