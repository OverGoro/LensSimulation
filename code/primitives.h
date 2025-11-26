#ifndef PRIMITIVES_H
#define PRIMITIVES_H

#include "qdebug.h"
#include <cmath>
#include <ostream>

class Vec3 {
public:
    float x, y, z;


    Vec3();
    Vec3(float x, float y, float z);

    Vec3 (const Vec3 &other);
    Vec3 &operator=(const Vec3 &other);

    Vec3 operator*(float b) const;
    Vec3 operator/(float b) const;
    Vec3& operator*=(float b);
    Vec3& operator*=(const Vec3& b);
    Vec3& operator/=(float b);

    bool operator==(const Vec3 &other) const;

    Vec3 operator+(const Vec3& other) const;
    Vec3& operator+=(const Vec3& other);

    Vec3& operator+= (const float &val);


    Vec3 operator-() const;
    Vec3 operator-(const Vec3& other) const;
    Vec3& operator-=(const Vec3& other);


    Vec3 operator*(const Vec3& other) const;

    float operator [](const int &axes) const;


    Vec3 normalize() const;

    Vec3 cross(const Vec3& other) const;

    float length() const;
    float lengthSquared() const;


    float dot(const Vec3& other) const;

    static Vec3 randomUnitVector();
    static Vec3 getRandomDirection();
    static Vec3 sampleHemisphere(const Vec3 &normal);

    static Vec3 rotateAroundAxis(const Vec3 &v, const Vec3 &axis, float angle);
    static Vec3 reflect(const Vec3 &incident, const Vec3 &normal);
    static Vec3 refract(const Vec3 &incident, const Vec3 &normal, float etai, float etat);
    static Vec3 refract(const Vec3& incident, const Vec3& normal, float eta);

    static float fresnel(const Vec3 &incident, const Vec3 &normal, float ior);

    friend std::ostream& operator<<(std::ostream &os, const Vec3 &vec) {
        os << "Vec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
        return os;
    }
    friend QDebug operator<<(QDebug dbg, const Vec3 &vec) {
        dbg.nospace() << "Vec3(" << vec.x << ", " << vec.y << ", " << vec.z << ")";
        return dbg.space();
    }

};



struct LightColor {
    Vec3 color;         // Цвет света
    float intensity;    // Интенсивность света
};

struct GraphicParams{
    Vec3 _color;
    float _transparency;
    float _refractiveIndex;
    float _reflectivity;
    Vec3 _normal;
    LightColor _emission = {Vec3(0.0f, 0.0f, 0.0f), 0};
    float _refractionDeltaR = -0.005f;
    float _refractionDeltaB = +0.004f;
    GraphicParams();
};

struct Ray {
    Ray(){};
    Vec3 origin;
    Vec3 direction;
    bool insideObject;
    float previousRefraction;

    Ray(const Vec3& o, const Vec3& d, bool inside = false, float refraction = 1.0f);
};


#endif // PRIMITIVES_H
