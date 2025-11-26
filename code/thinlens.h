#ifndef THIN_LENS_H
#define THIN_LENS_H

#include "baseobject.h"
#include "primitives.h"

class Lens : public BaseObject {
public:
    Lens(const Vec3& position, const Vec3& direction, float curveRadius, float radius, const GraphicParams& params);

    virtual bool intersect(const Ray& ray) const override;
    virtual bool intersect(const Ray& ray, float& t) const override;

    virtual Vec3 position() const override { return _position; };
    virtual void setPosition(const Vec3 &pos) override;
    virtual GraphicParams hitParams(const Ray& ray, float t) const override;

public:
    Vec3 _position;
    Vec3 _focalPos1, _focalPos2;
    Vec3 _direction;
    float _curveRadius;
    float _radius;
    Vec3 getNormal(const Vec3& point, const Vec3& sphereCenter) const;
    bool intersectWithSphere(const Ray& ray, const Vec3& center, float radius, float& t_min, float& t_max) const;
    Vec3 direction() const;
    void setDirection(const Vec3 &newDirection);
    float curveRadius() const;
    void setCurveRadius(float newCurveRadius);
    float radius() const;
    void setRadius(float newRadius);
};
#endif // THIN_LENS_H
