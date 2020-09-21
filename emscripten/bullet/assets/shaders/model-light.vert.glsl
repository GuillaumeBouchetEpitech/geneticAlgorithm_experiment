
precision mediump float;

// uniform mat4 u_composedMatrix;
uniform mat4 u_projectionMatrix;
uniform mat4 u_modelMatrix;
uniform mat4 u_viewMatrix;
uniform vec3 u_lightPos;

attribute vec3 a_position;
attribute vec3 a_color;
attribute vec3 a_normal;
attribute mat4 a_offsetTransform;
attribute vec3 a_offsetColor;

varying vec3 v_color;
varying vec3 v_normal;
varying vec3 v_vertPos;
varying vec3 v_lightPos;

void main(void)
{
	// gl_Position = u_composedMatrix * a_offsetTransform * vec4(a_position, 1.0);

	mat4 localTransform = u_viewMatrix * u_modelMatrix * a_offsetTransform;

	gl_Position = u_projectionMatrix * localTransform * vec4(a_position, 1.0);

	v_color = a_color * a_offsetColor;

	// v_normal = vec3(u_modelMatrix * a_offsetTransform * vec4(a_normal, 0.0));
	v_normal = vec3(localTransform * vec4(a_normal, 0.0));

	// vec4 vertPos4 = u_modelMatrix * a_offsetTransform * vec4(a_position, 1.0);
	vec4 vertPos4 = localTransform * vec4(a_position, 1.0);
	v_vertPos = vertPos4.xyz / vertPos4.w;

	v_lightPos = u_lightPos;
}
