#include "sphere.h"

Sphere::Sphere(const Vec3& c, float r, const Vec3& col, float transparency, float refractionIndex, float reflectivity)
    : _center(c), _radius(r)
{
    _params._color = col;
    _params._transparency = transparency;
    _params._refractiveIndex = refractionIndex;
    _params._reflectivity = reflectivity;

}

Sphere::Sphere(const Sphere& other)
{
    _center = other._center;
    _radius = other._radius;
    _params._color = other._params._color;
    _params._transparency = other._params._transparency;
    _params._refractiveIndex = other._params._refractiveIndex;
    _params._reflectivity = other._params._reflectivity;
    _params._emission = other._params._emission;
}

bool Sphere::intersect(const Ray& ray) const {
    Vec3 oc = ray.origin - _center;
    float half_b = oc.dot(ray.direction);
    float c = oc.dot(oc) - _radius * _radius;
    float discriminant = half_b * half_b - c;

    return discriminant >= 0;
}

bool Sphere::intersect(const Ray& ray, float& t) const {
    Vec3 oc = ray.origin - _center;
    float half_b = oc.dot(ray.direction);
    float c = oc.dot(oc) - _radius * _radius;
    float discriminant = half_b * half_b - c;

    if (discriminant < 0) return false;

    float sqrt_discriminant = std::sqrt(discriminant);
    t = -half_b - sqrt_discriminant;

    if (t < 0) t = -half_b + sqrt_discriminant;
    return t >= 0;
}

Vec3 Sphere::position() const {return _center;}

void Sphere::setPosition(const Vec3 &pos)
{
    _center = pos;
}

GraphicParams Sphere::hitParams(const Ray &ray, float t) const
{
    GraphicParams rp = _params;
    rp._normal = (ray.origin + ray.direction * t - _center).normalize();
    return rp;
}
