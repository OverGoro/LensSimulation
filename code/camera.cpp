#include "camera.h"

Camera::Camera()
{
    _position = Vec3(0,0,0);
    _direction = Vec3(1,0,0);
    _screenDistance = 1;
}

Camera::Camera(const Vec3 &pos, const Vec3 &dir, const double &dist, double fov)
{
    _position = pos;
    _direction = dir;
    _screenDistance = dist;
    _fov = fov;
}

Vec3 Camera::position() const
{
    return _position;
}

void Camera::setPosition(const Vec3 &newPosition)
{
    _position = newPosition;
}

Vec3 Camera::direction() const
{
    return _direction;
}

void Camera::setDirection(const Vec3 &newDirection)
{
    _direction = newDirection;
}

double Camera::screenDistance() const
{
    return _screenDistance;
}

void Camera::setScreenDistance(double newScreenDistance)
{
    _screenDistance = newScreenDistance;
}

double Camera::fov() const
{
    return _fov;
}

void Camera::setFov(double newFov)
{
    _fov = newFov;
}
