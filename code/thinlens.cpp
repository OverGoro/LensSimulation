#include "thinlens.h"
#include <cmath>
#include <limits>

Lens::Lens(const Vec3& position, const Vec3& direction, float curveRadius, float radius, const GraphicParams& params)
    : _position(position), _direction(direction.normalize()), _curveRadius(curveRadius), _radius(radius) {
    _params = params;
    _focalPos1 = position + direction.normalize() * sqrt(curveRadius * curveRadius - radius * radius);
    _focalPos2 = position - direction.normalize() * sqrt(curveRadius * curveRadius - radius * radius);
}

bool Lens::intersect(const Ray &ray) const
{
    float t1_min, t1_max, t2_min, t2_max;

    if (!intersectWithSphere(ray, _focalPos1, _curveRadius, t1_min, t1_max)) return false;
    if (!intersectWithSphere(ray, _focalPos2, _curveRadius, t2_min, t2_max)) return false;

    float t_enter = std::max(t1_min, t2_min);
    float t_exit = std::min(t1_max, t2_max);

    if (t_enter < t_exit && t_exit > 0) {
        return true;
    }

    return false; // Нет пересечения с областью пересечения сфер

}

bool Lens::intersect(const Ray& ray, float& t) const {
    float t1_min, t1_max, t2_min, t2_max;

    if (!intersectWithSphere(ray, _focalPos1, _curveRadius, t1_min, t1_max)) return false;
    if (!intersectWithSphere(ray, _focalPos2, _curveRadius, t2_min, t2_max)) return false;

    float t_enter = std::max(t1_min, t2_min);
    float t_exit = std::min(t1_max, t2_max);

    if (t_enter < t_exit && t_exit > 0) {
        t = (t_enter > 0) ? t_enter : t_exit; // Берем ближайшую положительную точку пересечения
        return true;
    }

    return false; // Нет пересечения с областью пересечения сфер
}

void Lens::setPosition(const Vec3 &pos)
{
    _position= pos;
    _focalPos1 = _position + _direction.normalize() * sqrt(_curveRadius * _curveRadius - _radius * _radius);
    _focalPos2 = _position - _direction.normalize() * sqrt(_curveRadius * _curveRadius - _radius * _radius);
}

GraphicParams Lens::hitParams(const Ray& ray, float t) const {
    // Определяем точку пересечения
    Vec3 intersectionPoint = ray.origin + ray.direction * t;

    // Проверяем, к какой сфере принадлежит точка пересечения и вычисляем нормаль
    Vec3 normal;
    if (fabs((intersectionPoint - _focalPos1).length() - _curveRadius) <= fabs((intersectionPoint - _focalPos2).length() - _curveRadius)){
        normal = getNormal(intersectionPoint, _focalPos1);
    } else {
        normal = getNormal(intersectionPoint, _focalPos2);
    }

    auto res = _params;
    res._normal = normal;
    return res;
}

float Lens::radius() const
{
    return _radius;
}

void Lens::setRadius(float newRadius)
{
    _radius = newRadius;
    _focalPos1 = _position + _direction.normalize() * sqrt(_curveRadius * _curveRadius - _radius * _radius);
    _focalPos2 = _position - _direction.normalize() * sqrt(_curveRadius * _curveRadius - _radius * _radius);

}

float Lens::curveRadius() const
{
    return _curveRadius;
}

void Lens::setCurveRadius(float newCurveRadius)
{
    _curveRadius = newCurveRadius;
    _focalPos1 = _position + _direction.normalize() * sqrt(_curveRadius * _curveRadius - _radius * _radius);
    _focalPos2 = _position - _direction.normalize() * sqrt(_curveRadius * _curveRadius - _radius * _radius);
}

Vec3 Lens::direction() const
{
    return _direction;
}

void Lens::setDirection(const Vec3 &newDirection)
{
    _direction = newDirection;
    _focalPos1 = _position + _direction.normalize() * sqrt(_curveRadius * _curveRadius - _radius * _radius);
    _focalPos2 = _position - _direction.normalize() * sqrt(_curveRadius * _curveRadius - _radius * _radius);
}

Vec3 Lens::getNormal(const Vec3& point, const Vec3& sphereCenter) const {
    Vec3 normal = point - sphereCenter;
    return normal.normalize();
}

bool Lens::intersectWithSphere(const Ray& ray, const Vec3& center, float radius, float& t_min, float& t_max) const {
    Vec3 oc = ray.origin - center;
    float half_b = oc.dot(ray.direction);
    float c = oc.dot(oc) - radius * radius;
    float discriminant = half_b * half_b - c;

    if (discriminant < 0) return false; // Нет пересечения

    float sqrt_discriminant = std::sqrt(discriminant);
    t_min = -half_b - sqrt_discriminant;
    t_max = -half_b + sqrt_discriminant;

    if (t_min > t_max) std::swap(t_min, t_max);

    return true;
}
