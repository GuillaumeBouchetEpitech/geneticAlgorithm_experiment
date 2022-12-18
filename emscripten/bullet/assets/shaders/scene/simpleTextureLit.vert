#version 300 es

precision highp float;

// uniform mat4 u_composedMatrix;
uniform mat4 u_projectionMatrix;
uniform mat4 u_modelViewMatrix;

in vec3 a_position;
in vec3 a_normal;
in vec2 a_texCoord;

out vec2 v_texCoord;
out vec3 v_normal;
out vec3 v_vertPos;

void main(void)
{
	vec4 vertPos4 = u_modelViewMatrix * vec4(a_position, 1.0);

	gl_Position = u_projectionMatrix * vertPos4;

	v_texCoord = a_texCoord;
	v_normal = vec3(u_modelViewMatrix * vec4(a_normal, 0.0));
	v_vertPos = vec3(vertPos4) / vertPos4.w;
}
