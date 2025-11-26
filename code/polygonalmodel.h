#ifndef __POLYGONALMODEL_H__
#define __POLYGONALMODEL_H__

#include <vector>
#include "polygon.h"

struct PolygonalModel : public BaseObject {
public:
    std::vector<Polygon> polygons;
    Vec3 boundingSphereCenter;
    float boundingSphereRadius;

    PolygonalModel(const std::vector<Polygon>& polys);

    virtual bool intersect(const Ray& ray) const override;
    virtual bool intersect(const Ray& ray, float& t) const override;

    virtual Vec3 position() const override;
    virtual void setPosition(const Vec3 &pos) override;
    virtual GraphicParams hitParams(const Ray& ray, float t) const override;

    void addPolygon(const Polygon& poly);
    void calculateBoundingSphere();
    void setRefractionIndex(double refrIndex);
    void setTransparency(double transparency);
    bool intersectBoundingSphere(const Ray& ray) const;
    void load(const QString &filename);
    void scale(double k);
    void rotate(const Vec3& axis, double angle);
};

#endif // __POLYGONALMODEL_H__
