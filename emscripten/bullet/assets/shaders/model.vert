
precision highp float;

uniform mat4 u_projectionMatrix;
uniform mat4 u_modelViewMatrix;

attribute vec3 a_vertex_position;
attribute vec3 a_vertex_color;
attribute vec3 a_vertex_normal;

attribute mat4 a_offset_transform;
attribute vec3 a_offset_color;

varying vec3 v_color;
varying vec3 v_normalInterp;
varying vec3 v_vertPos;

void main(void)
{
	mat4 transform = u_modelViewMatrix * a_offset_transform;
	vec4 vertPos4 = transform * vec4(a_vertex_position, 1.0);

	gl_Position = u_projectionMatrix * vertPos4;

	v_color = a_vertex_color * a_offset_color;
	v_vertPos = vec3(vertPos4) / vertPos4.w;
	v_normalInterp = vec3(transform * vec4(a_vertex_normal, 0.0));
}
