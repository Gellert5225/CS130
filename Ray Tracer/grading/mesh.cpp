#include "mesh.h"
#include <fstream>
#include <string>
#include <limits>
#include "plane.h"

// Consider a triangle to intersect a ray if the ray intersects the plane of the
// triangle with barycentric weights in [-weight_tolerance, 1+weight_tolerance]
static const double weight_tolerance = 1e-4;

// Read in a mesh from an obj file.  Populates the bounding box and registers
// one part per triangle (by setting number_parts).
void Mesh::Read_Obj(const char* file)
{
    std::ifstream fin(file);
    if(!fin)
    {
        exit(EXIT_FAILURE);
    }
    std::string line;
    ivec3 e;
    vec3 v;
    box.Make_Empty();
    while(fin)
    {
        getline(fin,line);

        if(sscanf(line.c_str(), "v %lg %lg %lg", &v[0], &v[1], &v[2]) == 3)
        {
            vertices.push_back(v);
            box.Include_Point(v);
        }

        if(sscanf(line.c_str(), "f %d %d %d", &e[0], &e[1], &e[2]) == 3)
        {
            for(int i=0;i<3;i++) e[i]--;
            triangles.push_back(e);
        }
    }
    number_parts=triangles.size();
}

// Check for an intersection against the ray.  See the base class for details.
Hit Mesh::Intersection(const Ray& ray, int part) const
{
    double dist = 0;
    Hit hit = {0, 0, 0};
    if (part < 0) {
        for (int i = 0; i < triangles.size(); i++) {
            if (Intersect_Triangle(ray, i, dist)) {
                    hit.object = this;
                    hit.dist = dist;
                    hit.part = i;
                    return hit;
            }
        }
    } else {
        if(Intersect_Triangle(ray, part, dist)) {
            hit.object = this;
            hit.dist = dist;
            hit.part = part;
        }
        return hit;
    }
}

// Compute the normal direction for the triangle with index part.
vec3 Mesh::Normal(const vec3& point, int part) const
{
    assert(part>=0);

    vec3 a = vertices[triangles[part][0]];
    vec3 b = vertices[triangles[part][1]];
    vec3 c = vertices[triangles[part][2]];

    return cross(b - a, c - a).normalized();
}

// This is a helper routine whose purpose is to simplify the implementation
// of the Intersection routine.  It should test for an intersection between
// the ray and the triangle with index tri.  If an intersection exists,
// record the distance and return true.  Otherwise, return false.
// This intersection should be computed by determining the intersection of
// the ray and the plane of the triangle.  From this, determine (1) where
// along the ray the intersection point occurs (dist) and (2) the barycentric
// coordinates within the triangle where the intersection occurs.  The
// triangle intersects the ray if dist>small_t and the barycentric weights are
// larger than -weight_tolerance.  The use of small_t avoid the self-shadowing
// bug, and the use of weight_tolerance prevents rays from passing in between
// two triangles.
bool Mesh::Intersect_Triangle(const Ray& ray, int tri, double& dist) const
{
    vec3 a = vertices[triangles[tri][0]];
    vec3 b = vertices[triangles[tri][1]];
    vec3 c = vertices[triangles[tri][2]];
    vec3 p;

    Hit hit = Plane(a, Normal(a, tri)).Intersection(ray, tri);
    if (hit.object == nullptr) return false;

    p = ray.Point(hit.dist);

    // Cramer's rule
    vec3 v0 = b - a, v1 = c - a, v2 = p - a;
    double u, v, w;

    float d00 = dot(v0, v0);
    float d01 = dot(v0, v1);
    float d11 = dot(v1, v1);
    float d20 = dot(v2, v0);
    float d21 = dot(v2, v1);

    float denom = d00 * d11 - d01 * d01;

    v = (d11 * d20 - d01 * d21) / denom;
    w = (d00 * d21 - d01 * d20) / denom;
    u = 1.0f - v - w;

    if (u >= 0 && v >= 0 && w >= 0) {
        dist = hit.dist;
        return true;
    }

    return false;
}

// Compute the bounding box.  Return the bounding box of only the triangle whose
// index is part.
Box Mesh::Bounding_Box(int part) const
{
    Box b;
    //TODO;
    return b;
}
