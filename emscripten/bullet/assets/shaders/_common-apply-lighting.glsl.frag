
const float k_ambiant = 0.4;
const vec3 k_lightPos = vec3(0.0, 0.0, 0.0);
const vec3 k_specColor = vec3(1.0, 1.0, 1.0);

vec3 applyLighting(vec3 currentColor, vec3 normal, vec3 vertexPos)
{
    normal = normalize(normal);
    vec3 lightDir = normalize(k_lightPos - vertexPos);

    float lambertian = max(dot(lightDir, normal.xyz), 0.0);
    float specular = 0.0;

    if (lambertian > 0.0)
    {
        // lighting specular

        vec3 reflectDir = reflect(-lightDir, normal);
        vec3 viewDir = normalize(-vertexPos);

        float specAngle = max(dot(reflectDir, viewDir), 0.0);
        specular = pow(specAngle, 64.0);
    }

    // lighting output

    vec3 ambiant_color = currentColor * k_ambiant;
    vec3 diffuse_color = currentColor * lambertian;
    vec3 specular_color = k_specColor * specular;

    return ambiant_color + diffuse_color + specular_color;
}
