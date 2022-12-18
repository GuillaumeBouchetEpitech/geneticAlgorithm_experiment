#version 300 es

precision lowp float;

uniform sampler2D u_colorTexture;
uniform sampler2D u_outlineTexture;

out vec4 out_color;

void main(void)
{
  ivec2 pixelCoord = ivec2(gl_FragCoord.xy);
  vec4 pixelColor = texelFetch(u_colorTexture, pixelCoord, 0);
  vec4 pixelOutline = texelFetch(u_outlineTexture, pixelCoord, 0);

  if (pixelOutline.x == 0.0)
  {
    float depth = 0.0;
    depth += texelFetch(u_outlineTexture, pixelCoord, 0).x;
    depth += texelFetch(u_outlineTexture, pixelCoord + ivec2(-1, 0), 0).x;
    depth += texelFetch(u_outlineTexture, pixelCoord + ivec2(+1, 0), 0).x;
    depth += texelFetch(u_outlineTexture, pixelCoord + ivec2( 0,-1), 0).x;
    depth += texelFetch(u_outlineTexture, pixelCoord + ivec2( 0,+1), 0).x;

    depth += texelFetch(u_outlineTexture, pixelCoord + ivec2(-1,-1), 0).x;
    depth += texelFetch(u_outlineTexture, pixelCoord + ivec2(+1,-1), 0).x;
    depth += texelFetch(u_outlineTexture, pixelCoord + ivec2(-1,+1), 0).x;
    depth += texelFetch(u_outlineTexture, pixelCoord + ivec2(+1,+1), 0).x;

    const vec4 outlineColor = vec4(1.0, 1.0, 1.0, 1.0);

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
