
precision highp float;

uniform mat4 u_composedMatrix;
uniform float u_alpha;
uniform float u_lowerLimit;
uniform float u_upperLimit;

attribute vec3 a_position;
attribute vec3 a_color;
attribute vec3 a_animated_normal;
attribute float a_index;

varying vec4 v_color;

const vec4 k_white = vec4(1.0);

void main(void)
{
	float deformationCoef = 0.0;

	if (a_index <= u_lowerLimit)
	{
		// not deformed (normal shape)
		deformationCoef = 0.0;
	}
	else if (a_index > u_upperLimit)
	{
		// fully deformed (shattered shape)
		deformationCoef = 1.0;
	}
	else
	{
		// animated deformation (normal shape <-> shattered shape)
		deformationCoef = (a_index - u_lowerLimit) / (u_upperLimit - u_lowerLimit);
	}

	vec3 position = a_position;

	if (deformationCoef == 1.0)
	{
		// invisible
		v_color = vec4(0.0);
	}
	else
	{
		// modified color (normal color <-> white color)
		v_color = mix(vec4(a_color, u_alpha), k_white, deformationCoef);

		// modified shape (normal shape <-> shattered shape)
		position -= a_animated_normal * deformationCoef;
	}

	gl_Position = u_composedMatrix * vec4(position, 1.0);
}
