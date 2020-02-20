varying vec4 position;
varying vec3 normal;
varying vec3 light_direction;

void main() {
    vec3 reflection = normalize(-light_direction + 2.0 * dot(light_direction, normal) * normal);

    vec4 ambient = gl_LightModel.ambient * gl_FrontMaterial.ambient;
    vec4 diffuse = gl_LightSource[0].diffuse * gl_FrontMaterial.diffuse * max(dot(normal, light_direction), 0.0);
    vec4 specular = gl_LightSource[0].specular * gl_FrontMaterial.specular * pow(max(dot(reflection, -normalize(position.xyz)), 0.0), gl_FrontMaterial.shininess);
    gl_FragColor = ambient + diffuse + specular;
}