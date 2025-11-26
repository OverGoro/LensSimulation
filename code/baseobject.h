#ifndef BASEOBJECT_H
#define BASEOBJECT_H

#include "primitives.h"

class BaseObject
{
public:
    GraphicParams _params;
    BaseObject(){};

    virtual bool intersect(const Ray& ray) const = 0;
    virtual bool intersect(const Ray& ray, float& t) const = 0;
    virtual Vec3 position() const = 0;
    virtual void setPosition(const Vec3 &) = 0;
    virtual GraphicParams hitParams(const Ray& ray, float t) const = 0;

};
#endif // BASEOBJECT_H
