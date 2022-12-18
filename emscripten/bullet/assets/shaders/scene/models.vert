#version 300 es

precision highp float;

uniform mat4 u_composedMatrix;

in vec3 a_vertex_position;
in vec3 a_vertex_color;

in mat4 a_offset_transform;
in vec3 a_offset_color;

out vec3 v_color;

void main(void)
{
	gl_Position = u_composedMatrix * a_offset_transform * vec4(a_vertex_position, 1.0);
	v_color = a_vertex_color * a_offset_color;
}
