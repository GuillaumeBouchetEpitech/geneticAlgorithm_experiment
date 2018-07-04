
uniform float u_Alpha;

varying vec3 v_Color;

void main()
{
	gl_FragColor = vec4(v_Color, u_Alpha);
}
