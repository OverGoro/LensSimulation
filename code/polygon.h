#ifndef __POLYGON_H__
#define __POLYGON_H__
#include "primitives.h"
#include "baseobject.h"
#include <ostream>
struct Polygon : public BaseObject{
public:
    Vec3 v0, v1, v2;

    Polygon(const Vec3& v0, const Vec3& v1, const Vec3& v2, const Vec3& color = Vec3(0,0,0), float transparency = 0.0f, float refractionIndex = 1.0f, float reflectivity = 0.0f);

    virtual bool intersect(const Ray& ray) const override;
    virtual bool intersect(const Ray& ray, float& t) const override;

    virtual Vec3 position() const override;;
    virtual void setPosition(const Vec3 &pos) override;
    void move(const Vec3 &delta);
    void rotate(const Vec3&axis, double angle);
    void rotate(const Vec3 &center, const Vec3 &axis, double angle);
    void scale(const Vec3& center, double k);
    virtual GraphicParams hitParams(const Ray& ray, float t) const override;
};

std::ostream& operator<<(std::ostream& os, const Polygon& polygon);

#endif // __POLYGON_H__
