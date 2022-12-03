
precision highp float;

// uniform mat4 u_composedMatrix;
uniform mat4 u_projectionMatrix;
uniform mat4 u_modelViewMatrix;

attribute vec3 a_position;
attribute vec3 a_normal;
attribute vec2 a_texCoord;

varying vec2 v_texCoord;
varying vec3 v_normalInterp;
varying vec3 v_vertPos;

void main(void)
{
	vec4 vertPos4 = u_modelViewMatrix * vec4(a_position, 1.0);

	gl_Position = u_projectionMatrix * vertPos4;

	v_texCoord = a_texCoord;
	v_vertPos = vec3(vertPos4) / vertPos4.w;
	v_normalInterp = vec3(u_modelViewMatrix * vec4(a_normal, 0.0));
}
