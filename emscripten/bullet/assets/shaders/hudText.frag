
precision lowp float;

uniform sampler2D u_texture;

varying vec2 v_texCoord;
varying vec3 v_color;

void main(void)
{
    vec4 texColor = texture2D(u_texture, v_texCoord);
    if (texColor.a < 0.5)
    {
        discard;
    }
    else
    {
        gl_FragColor = texColor * vec4(v_color, 1.0);
    }
}
