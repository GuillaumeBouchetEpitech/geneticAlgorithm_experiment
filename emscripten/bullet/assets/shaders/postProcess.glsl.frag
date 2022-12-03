precision lowp float;

uniform sampler2D u_colorTexture;

varying vec2 v_texCoord;

void main(void)
{

  gl_FragColor = texture2D(u_colorTexture, v_texCoord);

}
