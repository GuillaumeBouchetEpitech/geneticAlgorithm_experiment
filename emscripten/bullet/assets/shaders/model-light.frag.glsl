
precision mediump float;

varying vec3 v_color;
varying vec3 v_normal;
varying vec3 v_vertPos;
varying vec3 v_lightPos;

// const vec3 k_lightPos = vec3(0.0, 0.0, -100.0);
const vec3 k_lightPos = vec3(0.0, 0.0, 0.0);
const vec3 k_specColor = vec3(1.0, 1.0, 1.0);

void main(void)
{
	// gl_FragColor = vec4(v_color, 1.0);

	vec3 tmp_color = v_color;

	{ // lighting

		vec3 normal = normalize(v_normal);
		// vec3 lightDir = normalize(k_lightPos - v_vertPos);
		vec3 lightDir = normalize(v_vertPos - k_lightPos);
		// vec3 lightDir = normalize(v_vertPos - k_lightPos);

		float lambertian = max(dot(lightDir, v_normal.xyz), 0.0);
		float specular = 0.0;

		// // lighting specular
		// if (lambertian > 0.0)
		// {
		// 	vec3 reflectDir = reflect(-lightDir, normal);
		// 	vec3 viewDir = normalize(-v_vertPos);

		// 	float specAngle = max(dot(reflectDir, viewDir), 0.0);
		// 	// specular = pow(specAngle, 2.0);
		// 	specular = specAngle * 0.5;
		// }

		// lighting output
		tmp_color = tmp_color.xyz*0.15 + tmp_color.xyz*lambertian + specular*k_specColor;

	} // lighting

	gl_FragColor = vec4(tmp_color, 1.0);
}
