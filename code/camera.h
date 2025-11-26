#ifndef CAMERA_H
#define CAMERA_H

#include "primitives.h"

class Camera
{
public:
    Camera();
    Camera(const Vec3 &pos, const Vec3 &dir, const double &dist, double fov);
    Vec3 position() const;
    void setPosition(const Vec3 &newPosition);

    Vec3 direction() const;
    void setDirection(const Vec3 &newDirection);

    double screenDistance() const;
    void setScreenDistance(double newScreenDistance);

    double fov() const;
    void setFov(double newFov);

protected:
    Vec3 _position;
    Vec3 _direction;
    double _screenDistance;
    double _fov;
};

#endif // CAMERA_H
