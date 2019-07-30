
precision mediump float;

uniform mat4 u_composedMatrix;
uniform float u_alpha;
uniform float u_lowerLimit;
uniform float u_upperLimit;

attribute vec3 a_position;
attribute vec3 a_color;
attribute vec3 a_normal;
attribute float a_index;

varying vec4 v_color;

void main(void)
{
	float deformationCoef = 0.0;

	if (a_index <= u_lowerLimit)
	{
		// not deformed
		deformationCoef = 0.0;
	}
	else if (a_index > u_upperLimit)
	{
		// fully deformed
		deformationCoef = 1.0;
	}
	else
	{
		// animated deformation
		deformationCoef = (a_index - u_lowerLimit) / (u_upperLimit - u_lowerLimit);
	}

	vec3 position = a_position;

	if (deformationCoef == 1.0)
	{
		v_color = vec4(0.0);
	}
	else
	{
		v_color = mix(vec4(a_color, u_alpha), vec4(1.0), deformationCoef);

		if (deformationCoef > 0.0)
			position -= a_normal * deformationCoef;
	}

	gl_Position = u_composedMatrix * vec4(position, 1.0);
}
