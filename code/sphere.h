#ifndef __SPHERE_H__
#define __SPHERE_H__

#include <cmath>
#include <cmath>

#include "primitives.h"
#include "baseobject.h"

struct Sphere : public BaseObject
{
public:
    Vec3 _center;
    float _radius;

    Sphere(const Vec3& c, float r, const Vec3& col, float transparency = 0.0f, float refractionIndex = 1.0f, float reflectivity = 0.0f);

    Sphere(const Sphere& other);



    virtual bool intersect(const Ray& ray) const override;
    virtual bool intersect(const Ray& ray, float& t) const override;

    virtual Vec3 position() const override;
    virtual void setPosition(const Vec3 &) override;

    virtual GraphicParams hitParams(const Ray& ray, float t) const override;
};



#endif // __SPHERE_H__
