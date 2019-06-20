#version 300 es
precision mediump float;

layout(std140) uniform u_Matrices
{
	mat4 u_composedMatrix;
};

layout(std140) uniform u_Blending
{
	uniform float u_alpha;
};

layout(std140) uniform u_Animation
{
	uniform float u_lowerLimit;
	uniform float u_upperLimit;
};

in vec3 a_position;
in vec3 a_color;
in vec3 a_normal;
in float a_index;

out vec4 v_color;

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
