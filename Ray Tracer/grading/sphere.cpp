#include "sphere.h"
#include "ray.h"

// Determine if the ray intersects with the sphere
Hit Sphere::Intersection(const Ray& ray, int part) const
{
    Hit hit = {0, 0, 0};

    vec3 oc = ray.endpoint - center;
    double a = dot(ray.direction, ray.direction);
    double b = 2 * dot(oc, ray.direction);
    double c = dot(oc, oc) - radius * radius;

    if (b * b - 4 * a * c >= 0) hit = {this, (-b - sqrt(b * b - 4 * a * c)) / (2 * a), 1};

    return hit;
}

vec3 Sphere::Normal(const vec3& point, int part) const
{
    return (point - center).normalized();
}

Box Sphere::Bounding_Box(int part) const
{
    Box box;
    TODO; // calculate bounding box
    return box;
}
