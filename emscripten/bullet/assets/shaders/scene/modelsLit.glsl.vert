#version 300 es

precision highp float;

uniform mat4 u_projectionMatrix;
uniform mat4 u_modelViewMatrix;

in vec3 a_vertex_position;
in vec3 a_vertex_color;
in vec3 a_vertex_normal;

in mat4 a_offset_transform;
in vec4 a_offset_color;
in vec4 a_offset_outlineColor;

out vec4 v_color;
out vec4 v_outlineColor;
out vec3 v_normal;
out vec3 v_vertPos;

void main(void)
{
	mat4 transform = u_modelViewMatrix * a_offset_transform;
	vec4 vertPos4 = transform * vec4(a_vertex_position, 1.0);

	gl_Position = u_projectionMatrix * vertPos4;

	v_color = vec4(a_vertex_color * a_offset_color.rgb, a_offset_color.a);
	v_outlineColor = a_offset_outlineColor;
	v_normal = vec3(transform * vec4(a_vertex_normal, 0.0));
	v_vertPos = vec3(vertPos4) / vertPos4.w;
}
