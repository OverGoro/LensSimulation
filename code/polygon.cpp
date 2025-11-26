#include "polygon.h"
#include <iostream>
#include <cmath>
Polygon::Polygon(const Vec3 &v0, const Vec3 &v1, const Vec3 &v2, const Vec3 &col, float trans, float refr, float refl)
    : v0(v0), v1(v1), v2(v2){
    _params._normal = (v1 - v0).cross(v2 - v0).normalize();
    _params._color = (col);
    _params._transparency = trans;
    _params._reflectivity = (refl);
    _params._refractiveIndex = refr;
}

bool Polygon::intersect(const Ray &ray) const
{
    float t;
    Vec3 edge1 = v1 - v0;
    Vec3 edge2 = v2 - v0;
    Vec3 h = ray.direction.cross(edge2);
    float a = edge1.dot(h);

    if (fabs(a) < 1e-6) return false;

    float f = 1.0f / a;
    Vec3 s = ray.origin - v0;
    float u = f * s.dot(h);

    if (u < 0.0f || u > 1.0f) return false;

    Vec3 q = s.cross(edge1);
    float v = f * ray.direction.dot(q);

    if (v < 0.0f || u + v > 1.0f) return false;

    t = f * edge2.dot(q);
    return t > 1e-6;

}

bool Polygon::intersect(const Ray& ray, float& t) const {
    // if (!intersectBoundingSphere(ray)) return false;

    Vec3 edge1 = v1 - v0;
    Vec3 edge2 = v2 - v0;
    Vec3 h = ray.direction.cross(edge2);
    float a = edge1.dot(h);

    if (fabs(a) < 1e-6) return false;

    float f = 1.0f / a;
    Vec3 s = ray.origin - v0;
    float u = f * s.dot(h);

    if (u < 0.0f || u > 1.0f) return false;

    Vec3 q = s.cross(edge1);
    float v = f * ray.direction.dot(q);

    if (v < 0.0f || u + v > 1.0f) return false;

    t = f * edge2.dot(q);
    return t > 1e-6;
}

Vec3 Polygon::position() const { return v0; }

void Polygon::setPosition(const Vec3 &pos)
{
    Vec3 delta = pos - v0;
    v0 += delta;
    v1 += delta;
    v2 += delta;
}

void Polygon::move(const Vec3 &delta)
{
    v0 += delta;
    v1 += delta;
    v2 += delta;
}

void Polygon::scale(const Vec3 &center, double k)
{
    v0 = center + (center - v0) * k;
    v1 = center + (center - v1) * k;
    v2 = center + (center - v2) * k;
}

GraphicParams Polygon::hitParams(const Ray &ray, float t) const
{
    GraphicParams rp = _params;
    if (rp._normal.dot(ray.direction) > 0) {
        rp._normal = -rp._normal;
    }
    return rp;
}

std::ostream& operator<<(std::ostream& os, const Polygon& polygon)
{
    os << "Polygon: v0 = (" << polygon.v0.x << ", " << polygon.v0.y << ", " << polygon.v0.z << "), v1 = (" << polygon.v1.x << ", " << polygon.v1.y << ", " << polygon.v1.z << "), v2 = (" << polygon.v2.x << ", " << polygon.v2.y << ", " << polygon.v2.z << ")";
    return os;
}

void Polygon::rotate(const Vec3& axis, double angle) {
    // Нормализуем ось вращения
    Vec3 normalizedAxis = axis.normalize();

    // Преобразуем угол в радианы
    double radians = angle * M_PI / 180.0;

    // Вычисляем компоненты матрицы поворота
    double cosTheta = std::cos(radians);
    double sinTheta = std::sin(radians);
    double oneMinusCosTheta = 1.0 - cosTheta;

    // Элементы матрицы поворота
    double m00 = cosTheta + normalizedAxis.x * normalizedAxis.x * oneMinusCosTheta;
    double m01 = normalizedAxis.x * normalizedAxis.y * oneMinusCosTheta - normalizedAxis.z * sinTheta;
    double m02 = normalizedAxis.x * normalizedAxis.z * oneMinusCosTheta + normalizedAxis.y * sinTheta;

    double m10 = normalizedAxis.y * normalizedAxis.x * oneMinusCosTheta + normalizedAxis.z * sinTheta;
    double m11 = cosTheta + normalizedAxis.y * normalizedAxis.y * oneMinusCosTheta;
    double m12 = normalizedAxis.y * normalizedAxis.z * oneMinusCosTheta - normalizedAxis.x * sinTheta;

    double m20 = normalizedAxis.z * normalizedAxis.x * oneMinusCosTheta - normalizedAxis.y * sinTheta;
    double m21 = normalizedAxis.z * normalizedAxis.y * oneMinusCosTheta + normalizedAxis.x * sinTheta;
    double m22 = cosTheta + normalizedAxis.z * normalizedAxis.z * oneMinusCosTheta;

    // Применяем матрицу поворота к каждой вершине полигона
    Vec3 vertices[3] = {v0, v1, v2};
    for (int i = 0; i < 3; ++i) {
        Vec3& v = vertices[i];
        double x = v.x;
        double y = v.y;
        double z = v.z;

        v.x = m00 * x + m01 * y + m02 * z;
        v.y = m10 * x + m11 * y + m12 * z;
        v.z = m20 * x + m21 * y + m22 * z;
    }

    // Обновляем вершины
    v0 = vertices[0];
    v1 = vertices[1];
    v2 = vertices[2];
}

void Polygon::rotate(const Vec3 &center, const Vec3 &axis, double angle)
{
    // Нормализуем ось вращения
    Vec3 normalizedAxis = axis.normalize();

    // Преобразуем угол в радианы
    double radians = angle * M_PI / 180.0;

    // Вычисляем компоненты матрицы поворота
    double cosTheta = std::cos(radians);
    double sinTheta = std::sin(radians);
    double oneMinusCosTheta = 1.0 - cosTheta;

    // Элементы матрицы поворота
    double m00 = cosTheta + normalizedAxis.x * normalizedAxis.x * oneMinusCosTheta;
    double m01 = normalizedAxis.x * normalizedAxis.y * oneMinusCosTheta - normalizedAxis.z * sinTheta;
    double m02 = normalizedAxis.x * normalizedAxis.z * oneMinusCosTheta + normalizedAxis.y * sinTheta;

    double m10 = normalizedAxis.y * normalizedAxis.x * oneMinusCosTheta + normalizedAxis.z * sinTheta;
    double m11 = cosTheta + normalizedAxis.y * normalizedAxis.y * oneMinusCosTheta;
    double m12 = normalizedAxis.y * normalizedAxis.z * oneMinusCosTheta - normalizedAxis.x * sinTheta;

    double m20 = normalizedAxis.z * normalizedAxis.x * oneMinusCosTheta - normalizedAxis.y * sinTheta;
    double m21 = normalizedAxis.z * normalizedAxis.y * oneMinusCosTheta + normalizedAxis.x * sinTheta;
    double m22 = cosTheta + normalizedAxis.z * normalizedAxis.z * oneMinusCosTheta;

    // Применяем поворот относительно центра
    Vec3 vertices[3] = {v0, v1, v2};
    for (int i = 0; i < 3; ++i) {
        Vec3& v = vertices[i];

        // Перемещаем вершину к центру
        Vec3 translated = v - center;

        // Применяем матрицу поворота
        double x = translated.x;
        double y = translated.y;
        double z = translated.z;

        translated.x = m00 * x + m01 * y + m02 * z;
        translated.y = m10 * x + m11 * y + m12 * z;
        translated.z = m20 * x + m21 * y + m22 * z;

        // Возвращаем вершину на место
        v = translated + center;
    }

    // Обновляем вершины
    v0 = vertices[0];
    v1 = vertices[1];
    v2 = vertices[2];
}


