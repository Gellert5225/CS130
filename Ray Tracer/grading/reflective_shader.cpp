#include "reflective_shader.h"
#include "ray.h"
#include "render_world.h"

vec3 Reflective_Shader::
Shade_Surface(const Ray& ray,const vec3& intersection_point,
    const vec3& normal,int recursion_depth) const
{
    vec3 color;
    vec3 reflection = ray.direction - 2 * (dot(ray.direction, normal)) * normal;
    Ray reflectionRay(intersection_point, reflection);

    color = (1 - reflectivity) * shader->Shade_Surface(ray, intersection_point, normal, recursion_depth);
    if(recursion_depth >= 1 && recursion_depth < world.recursion_depth_limit) {
        color += (reflectivity * world.Cast_Ray(reflectionRay, ++recursion_depth));
    }
    return color;
}