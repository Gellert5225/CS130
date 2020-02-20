#include "light.h"
#include "phong_shader.h"
#include "ray.h"
#include "render_world.h"
#include "object.h"

vec3 Phong_Shader::
Shade_Surface(const Ray& ray,const vec3& intersection_point,
    const vec3& normal,int recursion_depth) const
{
    vec3 color, light_direction, reflection, diffuse, specular, ambient;

    ambient = world.ambient_color * world.ambient_intensity * color_ambient;

    for(unsigned int i = 0; i < world.lights.size(); i++) {
        light_direction = world.lights[i]->position - intersection_point;
        reflection = (-light_direction + 2 * dot(light_direction, normal) * normal).normalized();
        
        vec3 light_color = world.lights[i]->Emitted_Light(light_direction);

        if (world.enable_shadows) {
            Ray shadowRay;
            shadowRay.endpoint = intersection_point;
            shadowRay.direction = light_direction.normalized();
            Hit hitObject = world.Closest_Intersection(shadowRay); 
            
            if (hitObject.object != nullptr && hitObject.dist < light_direction.magnitude()) // if the shadow ray intersects and object is in front of light
                continue; // dont render diffuse or specular lighting, it's a shadow.
        }

        diffuse += light_color * color_diffuse * std::max(dot(normal, light_direction.normalized()), 0.0);
        specular += light_color * color_specular * std::pow(std::max(dot(reflection, -(ray.direction.normalized())), 0.0), specular_power);
    }

    color += diffuse + specular + ambient;

    return color;
}
