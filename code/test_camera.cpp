#include "sphere.h"
#include "thinlens.h"
#ifdef TESTING
#include "camera.h"
#include "primitives.h"
#include "qobject.h"
#include "polygon.h"
#include "QTest"
class TestAll : public QObject
{
    Q_OBJECT

private slots:
    void testDefaultConstructor();
    void testParameterizedConstructor();
    void testSetPosition();
    void testSetDirection();
    void testSetScreenDistance();
    void testSetFov();

    // Тесты для Polygon
    void testPolygonDefaultConstructor();
    void testPolygonParameterizedConstructor();
    void testPolygonSetPosition();
    void testPolygonIntersect();
    void testPolygonIntersectWithT();
    void testPolygonHitParams();

    // для vec3
    void testVec3DefaultConstructor();
    void testVec3ParameterizedConstructor();
    void testVec3CopyConstructor();
    void testVec3AssignmentOperator();
    void testVec3Addition();
    void testVec3Subtraction();
    void testVec3Multiplication();
    void testVec3Division();
    void testVec3DotProduct();
    void testVec3CrossProduct();
    void testVec3Normalization();
    void testVec3Length();
    void testVec3LengthSquared();
    void testVec3RandomUnitVector();
    void testVec3Reflect();
    void testVec3Refract();

    void testSphereDefaultConstructor();
    void testSphereParameterizedConstructor();
    void testSphereCopyConstructor();
    void testSphereIntersect();
    void testSphereIntersectWithDistance();
    void testSpherePosition();
    void testSphereSetPosition();
    void testSphereHitParams();

    void testLensParameterizedConstructor();
    void testLensCopyConstructor();
    void testLensIntersect();
    void testLensIntersectWithDistance();
    void testLensPosition();
    void testLensSetPosition();
    void testLensHitParams();
    void testLensSetDirection();
    void testLensSetRadius();
    void testLensSetCurveRadius();

};

void TestAll::testLensParameterizedConstructor()
{
    Vec3 position(0.0f, 0.0f, 0.0f);
    Vec3 direction(0.0f, 0.0f, 1.0f);
    float curveRadius = 5.0f;
    float radius = 2.0f;
    GraphicParams params;
    params._color = Vec3(1.0f, 1.0f, 1.0f); // Белый цвет

    Lens lens(position, direction, curveRadius, radius, params);

    QCOMPARE(lens.position(), position);
    QCOMPARE(lens.direction(), direction);
    QCOMPARE(lens.curveRadius(), curveRadius);
    QCOMPARE(lens.radius(), radius);
}

void TestAll::testLensCopyConstructor()
{
    Vec3 position(0.0f, 0.0f, 0.0f);
    Vec3 direction(0.0f, 0.0f, 1.0f);
    float curveRadius = 5.0f;
    float radius = 2.0f;
    GraphicParams params;
    params._color = Vec3(1.0f, 1.0f, 1.0f); // Белый цвет

    Lens lens1(position, direction, curveRadius, radius, params);
    Lens lens2(lens1);

    QCOMPARE(lens2.position(), lens1.position());
    QCOMPARE(lens2.direction(), lens1.direction());
    QCOMPARE(lens2.curveRadius(), lens1.curveRadius());
    QCOMPARE(lens2.radius(), lens1.radius());
}

void TestAll::testLensIntersect()
{
    Vec3 position(0.0f, 0.0f, 0.0f);
    Vec3 direction(0.0f, 0.0f, 1.0f);
    float curveRadius = 5.0f;
    float radius = 2.0f;
    GraphicParams params;
    params._color = Vec3(1.0f, 1.0f, 1.0f); // Белый цвет

    Lens lens(position, direction, curveRadius, radius, params);

    Ray ray;
    ray.origin = Vec3(0.0f, 0.0f, -10.0f);
    ray.direction = Vec3(0.0f, 0.0f, 1.0f).normalize();

    bool result = lens.intersect(ray);
    QCOMPARE(result, true); // Ожидаем пересечения
}

void TestAll::testLensIntersectWithDistance()
{
    Vec3 position(0.0f, 0.0f, 0.0f);
    Vec3 direction(0.0f, 0.0f, 1.0f);
    float curveRadius = 5.0f;
    float radius = 2.0f;
    GraphicParams params;
    params._color = Vec3(1.0f, 1.0f, 1.0f); // Белый цвет

    Lens lens(position, direction, curveRadius, radius, params);

    Ray ray;
    ray.origin = Vec3(0.0f, 0.0f, -10.0f);
    ray.direction = Vec3(0.0f, 0.0f, 1.0f).normalize();

    float t;
    bool result = lens.intersect(ray, t);
    QCOMPARE(result, true); // Ожидаем пересечения
    QCOMPARE(t > 0, true); // t должен быть положительным
}

void TestAll::testLensPosition()
{
    Vec3 position(1.0f, 2.0f, 3.0f);
    Vec3 direction(0.0f, 0.0f, 1.0f);
    float curveRadius = 5.0f;
    float radius = 2.0f;
    GraphicParams params;
    params._color = Vec3(1.0f, 1.0f, 1.0f); // Белый цвет

    Lens lens(position, direction, curveRadius, radius, params);

    QCOMPARE(lens.position(), position);
}

void TestAll::testLensSetPosition()
{
    Vec3 position(1.0f, 2.0f, 3.0f);
    Vec3 direction(0.0f, 0.0f, 1.0f);
    float curveRadius = 5.0f;
    float radius = 2.0f;
    GraphicParams params;
    params._color = Vec3(1.0f, 1.0f, 1.0f); // Белый цвет

    Lens lens(position, direction, curveRadius, radius, params);

    Vec3 newPosition(4.0f, 5.0f, 6.0f);
    lens.setPosition(newPosition);
    QCOMPARE(lens.position(), newPosition);
}

void TestAll::testLensHitParams()
{
    Vec3 position(0.0f, 0.0f, 0.0f);
    Vec3 direction(0.0f, 0.0f, 1.0f);
    float curveRadius = 5.0f;
    float radius = 2.0f;
    GraphicParams params;
    params._color = Vec3(1.0f, 1.0f, 1.0f); // Белый цвет

    Lens lens(position, direction, curveRadius, radius, params);

    Ray ray;
    ray.origin = Vec3(0.0f, 0.0f, -10.0f);
    ray.direction = Vec3(0.0f, 0.0f, 1.0f).normalize();

    float t;
    lens.intersect(ray, t);
    GraphicParams hitParams = lens.hitParams(ray, t);

    // Проверяем, что нормаль корректно вычислена
    Vec3 intersectionPoint = ray.origin + ray.direction * t;
    Vec3 expectedNormal = lens.getNormal(intersectionPoint, lens._focalPos1);
    QCOMPARE(hitParams._normal, expectedNormal);
}

void TestAll::testLensSetDirection()
{
    Vec3 position(0.0f, 0.0f, 0.0f);
    Vec3 direction(0.0f, 0.0f, 1.0f);
    float curveRadius = 5.0f;
    float radius = 2.0f;
    GraphicParams params;
    params._color = Vec3(1.0f, 1.0f, 1.0f); // Белый цвет

    Lens lens(position, direction, curveRadius, radius, params);

    Vec3 newDirection(1.0f, 0.0f, 0.0f);
    lens.setDirection(newDirection);
    QCOMPARE(lens.direction(), newDirection);
}

void TestAll::testLensSetRadius()
{
    Vec3 position(0.0f, 0.0f, 0.0f);
    Vec3 direction(0.0f, 0.0f, 1.0f);
    float curveRadius = 5.0f;
    float radius = 2.0f;
    GraphicParams params;
    params._color = Vec3(1.0f, 1.0f, 1.0f); // Белый цвет

    Lens lens(position, direction, curveRadius, radius, params);

    float newRadius = 3.0f;
    lens.setRadius(newRadius);
    QCOMPARE(lens.radius(), newRadius);
}

void TestAll::testLensSetCurveRadius()
{
    Vec3 position(0.0f, 0.0f, 0.0f);
    Vec3 direction(0.0f, 0.0f, 1.0f);
    float curveRadius = 5.0f;
    float radius = 2.0f;
    GraphicParams params;
    params._color = Vec3(1.0f, 1.0f, 1.0f); // Белый цвет

    Lens lens(position, direction, curveRadius, radius, params);

    float newCurveRadius = 6.0f;
    lens.setCurveRadius(newCurveRadius);
    QCOMPARE(lens.curveRadius(), newCurveRadius);
}

void TestAll::testDefaultConstructor()
{
    Camera camera;
    QCOMPARE(camera.position(), Vec3(0, 0, 0));
    QCOMPARE(camera.direction(), Vec3(1, 0, 0));
    QCOMPARE(camera.screenDistance(), 1.0);
    QCOMPARE(camera.fov(), 0.0); // Предполагаем, что по умолчанию FOV равен 0
}

void TestAll::testParameterizedConstructor()
{
    Vec3 position(1, 2, 3);
    Vec3 direction(0, 1, 0);
    double distance = 5.0;
    double fov = 90.0;

    Camera camera(position, direction, distance, fov);

    QCOMPARE(camera.position(), position);
    QCOMPARE(camera.direction(), direction);
    QCOMPARE(camera.screenDistance(), distance);
    QCOMPARE(camera.fov(), fov);
}

void TestAll::testSetPosition()
{
    Camera camera;
    Vec3 newPosition(10, 10, 10);
    camera.setPosition(newPosition);
    QCOMPARE(camera.position(), newPosition);
}

void TestAll::testSetDirection()
{
    Camera camera;
    Vec3 newDirection(0, 1, 0);
    camera.setDirection(newDirection);
    QCOMPARE(camera.direction(), newDirection);
}

void TestAll::testSetScreenDistance()
{
    Camera camera;
    double newDistance = 10.0;
    camera.setScreenDistance(newDistance);
    QCOMPARE(camera.screenDistance(), newDistance);
}

void TestAll::testSetFov()
{
    Camera camera;
    double newFov = 45.0;
    camera.setFov(newFov);
    QCOMPARE(camera.fov(), newFov);
}

void TestAll::testPolygonDefaultConstructor()
{
    Vec3 v0(0, 0, 0);
    Vec3 v1(1, 0, 0);
    Vec3 v2(0, 1, 0);
    Polygon polygon(v0, v1, v2);

    QCOMPARE(polygon.v0, v0);
    QCOMPARE(polygon.v1, v1);
    QCOMPARE(polygon.v2, v2);
}

void TestAll::testPolygonParameterizedConstructor()
{
    Vec3 v0(0, 0, 0);
    Vec3 v1(1, 0, 0);
    Vec3 v2(0, 1, 0);
    Vec3 color(1, 0, 0);
    float transparency = 0.5f;
    float refractionIndex = 1.5f;
    float reflectivity = 0.8f;

    Polygon polygon(v0, v1, v2, color, transparency, refractionIndex, reflectivity);

    QCOMPARE(polygon.v0, v0);
    QCOMPARE(polygon.v1, v1);
    QCOMPARE(polygon.v2, v2);
    QCOMPARE(polygon._params._color, color);
    QCOMPARE(polygon._params._transparency, transparency);
    QCOMPARE(polygon._params._refractiveIndex, refractionIndex);
    QCOMPARE(polygon._params._reflectivity, reflectivity);
}

void TestAll::testPolygonSetPosition()
{
    Vec3 v0(0, 0, 0);
    Vec3 v1(1, 0, 0);
    Vec3 v2(0, 1, 0);
    Polygon polygon(v0, v1, v2);

    Vec3 newPos(1, 1, 1);
    polygon.setPosition(newPos);

    QCOMPARE(polygon.v0, newPos);
    QCOMPARE(polygon.v1, newPos + Vec3(1, 0, 0));
    QCOMPARE(polygon.v2, newPos + Vec3(0, 1, 0));
}

void TestAll::testPolygonIntersect()
{
    Vec3 v0(0, 0, 0);
    Vec3 v1(1, 0, 0);
    Vec3 v2(0, 1, 0);
    Polygon polygon(v0, v1, v2);

    Ray ray(Vec3(0.5f, 0.5f, -1), Vec3(0, 0, 1)); // Луч, направленный на плоскость

    QVERIFY(polygon.intersect(ray)); // Проверяем, что пересечение происходит
}

void TestAll::testPolygonIntersectWithT()
{
    Vec3 v0(0, 0, 0);
    Vec3 v1(1, 0, 0);
    Vec3 v2(0, 1, 0);
    Polygon polygon(v0, v1, v2);

    Ray ray(Vec3(0.5f, 0.5f, -1), Vec3(0, 0, 1)); // Луч, направленный на плоскость
    float t;

    QVERIFY(polygon.intersect(ray, t)); // Проверяем, что пересечение происходит и возвращает t
    QVERIFY(t > 0); // t должно быть положительным
}

void TestAll::testPolygonHitParams()
{
    Vec3 v0(0, 0, 0);
    Vec3 v1(1, 0, 0);
    Vec3 v2(0, 1, 0);
    Polygon polygon(v0, v1, v2, Vec3(1, 0, 0));

    Ray ray(Vec3(0.5f, 0.5f, -1), Vec3(0, 0, 1));
    float t = 1.0f; // Положение пересечения

    GraphicParams params = polygon.hitParams(ray, t);
    QCOMPARE(params._color, Vec3(1, 0, 0));
}

// Тесты для Vec3

void TestAll::testVec3DefaultConstructor()
{
    Vec3 vec;
    QCOMPARE(vec.x, 0.0f);
    QCOMPARE(vec.y, 0.0f);
    QCOMPARE(vec.z, 0.0f);
}

void TestAll::testVec3ParameterizedConstructor()
{
    Vec3 vec(1.0f, 2.0f, 3.0f);
    QCOMPARE(vec.x, 1.0f);
    QCOMPARE(vec.y, 2.0f);
    QCOMPARE(vec.z, 3.0f);
}

void TestAll::testVec3CopyConstructor()
{
    Vec3 vec1(1.0f, 2.0f, 3.0f);
    Vec3 vec2(vec1);
    QCOMPARE(vec2.x, vec1.x);
    QCOMPARE(vec2.y, vec1.y);
    QCOMPARE(vec2.z, vec1.z);
}

void TestAll::testVec3AssignmentOperator()
{
    Vec3 vec1(1.0f, 2.0f, 3.0f);
    Vec3 vec2;
    vec2 = vec1;
    QCOMPARE(vec2.x, vec1.x);
    QCOMPARE(vec2.y, vec1.y);
    QCOMPARE(vec2.z, vec1.z);
}

void TestAll::testVec3Addition()
{
    Vec3 vec1(1.0f, 2.0f, 3.0f);
    Vec3 vec2(4.0f, 5.0f, 6.0f);
    Vec3 result = vec1 + vec2;
    QCOMPARE(result.x, 5.0f);
    QCOMPARE(result.y, 7.0f);
    QCOMPARE(result.z, 9.0f);
}

void TestAll::testVec3Subtraction()
{
    Vec3 vec1(4.0f, 5.0f, 6.0f);
    Vec3 vec2(1.0f, 2.0f, 3.0f);
    Vec3 result = vec1 - vec2;
    QCOMPARE(result.x, 3.0f);
    QCOMPARE(result.y, 3.0f);
    QCOMPARE(result.z, 3.0f);
}

void TestAll::testVec3Multiplication()
{
    Vec3 vec(1.0f, 2.0f, 3.0f);
    Vec3 result = vec * 2.0f;
    QCOMPARE(result.x, 2.0f);
    QCOMPARE(result.y, 4.0f);
    QCOMPARE(result.z, 6.0f);
}

void TestAll::testVec3Division()
{
    Vec3 vec(2.0f, 4.0f, 6.0f);
    Vec3 result = vec / 2.0f;
    QCOMPARE(result.x, 1.0f);
    QCOMPARE(result.y, 2.0f);
    QCOMPARE(result.z, 3.0f);
}

void TestAll::testVec3DotProduct()
{
    Vec3 vec1(1.0f, 2.0f, 3.0f);
    Vec3 vec2(4.0f, 5.0f, 6.0f);
    float result = vec1.dot(vec2);
    QCOMPARE(result, 32.0f); // 1*4 + 2*5 + 3*6
}

void TestAll::testVec3CrossProduct()
{
    Vec3 vec1(1.0f, 0.0f, 0.0f);
    Vec3 vec2(0.0f, 1.0f, 0.0f);
    Vec3 result = vec1.cross(vec2);
    QCOMPARE(result.x, 0.0f);
    QCOMPARE(result.y, 0.0f);
    QCOMPARE(result.z, 1.0f); // Ожидаемый результат: (0, 0, 1)
}

void TestAll::testVec3Normalization()
{
    Vec3 vec(3.0f, 4.0f, 0.0f);
    Vec3 normalized = vec.normalize();
    QCOMPARE(normalized.length(), 1.0f);
}

void TestAll::testVec3Length()
{
    Vec3 vec(3.0f, 4.0f, 0.0f);
    QCOMPARE(vec.length(), 5.0f); // Длина (3, 4, 0) должна быть 5
}

void TestAll::testVec3LengthSquared()
{
    Vec3 vec(3.0f, 4.0f, 0.0f);
    QCOMPARE(vec.lengthSquared(), 25.0f); // Длина в квадрате должна быть 25
}

void TestAll::testVec3RandomUnitVector()
{
    Vec3 vec = Vec3::randomUnitVector();
    QCOMPARE(vec.length(), 1.0f); // Длина случайного единичного вектора должна быть 1
}

void TestAll::testVec3Reflect()
{
    Vec3 incident(1.0f, -1.0f, 0.0f);
    Vec3 normal(0.0f, 1.0f, 0.0f);
    Vec3 reflected = Vec3::reflect(incident, normal);
    QCOMPARE(reflected.x, 1.0f);
    QCOMPARE(reflected.y, 1.0f);
    QCOMPARE(reflected.z, 0.0f); // Ожидаемый результат: (1, 1, 0)
}

void TestAll::testVec3Refract()
{
    Vec3 incident(1.0f, -1.0f, 0.0f);
    Vec3 normal(0.0f, 1.0f, 0.0f);
    float eta = 1.5f; // Примерный индекс преломления
    Vec3 refracted = Vec3::refract(incident, normal, eta);
    // Ожидаемое значение будет зависеть от расчетов преломления
    // Тест можно адаптировать в зависимости от ожидаемого результата
}

void TestAll::testSphereDefaultConstructor()
{
    // Параметры по умолчанию
    Vec3 center(0.0f, 0.0f, 0.0f);
    float radius = 1.0f;
    Vec3 color(1.0f, 0.0f, 0.0f); // Красный цвет
    Sphere sphere(center, radius, color);

    QCOMPARE(sphere.position(), center);
    QCOMPARE(sphere._radius, radius);
}

void TestAll::testSphereParameterizedConstructor()
{
    Vec3 center(1.0f, 2.0f, 3.0f);
    float radius = 4.0f;
    Vec3 color(0.0f, 1.0f, 0.0f); // Зеленый цвет
    Sphere sphere(center, radius, color);

    QCOMPARE(sphere.position(), center);
    QCOMPARE(sphere._radius, radius);
}

void TestAll::testSphereCopyConstructor()
{
    Vec3 center(1.0f, 2.0f, 3.0f);
    float radius = 4.0f;
    Vec3 color(0.0f, 1.0f, 0.0f); // Зеленый цвет
    Sphere sphere1(center, radius, color);
    Sphere sphere2(sphere1);

    QCOMPARE(sphere2.position(), sphere1.position());
    QCOMPARE(sphere2._radius, sphere1._radius);
}

void TestAll::testSphereIntersect()
{
    Vec3 center(0.0f, 0.0f, 0.0f);
    float radius = 1.0f;
    Sphere sphere(center, radius, Vec3(1.0f, 0.0f, 0.0f)); // Красный цвет

    Ray ray;
    ray.origin = Vec3(0.0f, 0.0f, -2.0f);
    ray.direction = Vec3(0.0f, 0.0f, 1.0f).normalize();

    bool result = sphere.intersect(ray);
    QCOMPARE(result, true); // Ожидаем пересечения
}

void TestAll::testSphereIntersectWithDistance()
{
    Vec3 center(0.0f, 0.0f, 0.0f);
    float radius = 1.0f;
    Sphere sphere(center, radius, Vec3(1.0f, 0.0f, 0.0f)); // Красный цвет

    Ray ray;
    ray.origin = Vec3(0.0f, 0.0f, -2.0f);
    ray.direction = Vec3(0.0f, 0.0f, 1.0f).normalize();

    float t;
    bool result = sphere.intersect(ray, t);
    QCOMPARE(result, true); // Ожидаем пересечения
    QCOMPARE(t >= 0, true); // t должен быть неотрицательным
}

void TestAll::testSpherePosition()
{
    Vec3 center(1.0f, 2.0f, 3.0f);
    Sphere sphere(center, 1.0f, Vec3(1.0f, 0.0f, 0.0f)); // Красный цвет

    QCOMPARE(sphere.position(), center);
}

void TestAll::testSphereSetPosition()
{
    Vec3 center(1.0f, 2.0f, 3.0f);
    Sphere sphere(center, 1.0f, Vec3(1.0f, 0.0f, 0.0f)); // Красный цвет

    Vec3 newPosition(4.0f, 5.0f, 6.0f);
    sphere.setPosition(newPosition);
    QCOMPARE(sphere.position(), newPosition);
}

void TestAll::testSphereHitParams()
{
    Vec3 center(0.0f, 0.0f, 0.0f);
    float radius = 1.0f;
    Sphere sphere(center, radius, Vec3(1.0f, 0.0f, 0.0f)); // Красный цвет

    Ray ray;
    ray.origin = Vec3(0.0f, 0.0f, -2.0f);
    ray.direction = Vec3(0.0f, 0.0f, 1.0f).normalize();

    float t;
    sphere.intersect(ray, t);
    GraphicParams params = sphere.hitParams(ray, t);

    // Проверяем, что нормаль направлена от центра сферы
    Vec3 expectedNormal = (ray.origin + ray.direction * t - center).normalize();
    QCOMPARE(params._normal, expectedNormal);
}


#include "test_camera.moc"
#endif
