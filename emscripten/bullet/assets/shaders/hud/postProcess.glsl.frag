#version 300 es

precision lowp float;

uniform sampler2D u_colorTexture;
uniform sampler2D u_outlineTexture;
uniform vec2 u_invResolution;

in vec2 v_texCoord;

out vec4 out_color;

void main(void)
{
  vec4 pixelColor = texture(u_colorTexture, v_texCoord);
  vec4 pixelOutline = texture(u_outlineTexture, v_texCoord);

  if (pixelOutline.x == 0.0)
  {
    float depth = 0.0;
    depth += texture(u_outlineTexture, v_texCoord).x;
    depth += texture(u_outlineTexture, v_texCoord + vec2(-1.0 * u_invResolution.x,  0.0 * u_invResolution.y)).x;
    depth += texture(u_outlineTexture, v_texCoord + vec2(+1.0 * u_invResolution.x,  0.0 * u_invResolution.y)).x;
    depth += texture(u_outlineTexture, v_texCoord + vec2( 0.0 * u_invResolution.x, -1.0 * u_invResolution.y)).x;
    depth += texture(u_outlineTexture, v_texCoord + vec2( 0.0 * u_invResolution.x, +1.0 * u_invResolution.y)).x;

    depth += texture(u_outlineTexture, v_texCoord + vec2(-1.0 * u_invResolution.x, -1.0 * u_invResolution.y)).x;
    depth += texture(u_outlineTexture, v_texCoord + vec2(+1.0 * u_invResolution.x, -1.0 * u_invResolution.y)).x;
    depth += texture(u_outlineTexture, v_texCoord + vec2(-1.0 * u_invResolution.x, +1.0 * u_invResolution.y)).x;
    depth += texture(u_outlineTexture, v_texCoord + vec2(+1.0 * u_invResolution.x, +1.0 * u_invResolution.y)).x;

    const vec4 outlineColor = vec4(0.8, 0.8, 0.8, 1.0);

    if (depth > 1.0)
      out_color = outlineColor;
    else
      out_color = pixelColor;
  }
  else
  {
    out_color = pixelColor;
  }
}
