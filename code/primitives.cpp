#include "primitives.h"
#include <ctime>
#include <random>
#include <algorithm>

Vec3::Vec3() : x(0), y(0), z(0) {}

Vec3::Vec3(float x, float y, float z) : x(x), y(y), z(z) {}

Vec3::Vec3(const Vec3 &other)
{
    x = other.x;
    y = other.y;
    z = other.z;
}

Vec3 &Vec3::operator=(const Vec3 &other)
{
    x = other.x;
    y = other.y;
    z = other.z;
    return *this;
}

Vec3 Vec3::operator*(float b) const {
    return Vec3(x * b, y * b, z * b);
}

Vec3 Vec3::operator/(float b) const {
    return Vec3(x / b, y / b, z / b);
}

Vec3& Vec3::operator*=(float b) {
    x *= b;
    y *= b;
    z *= b;
    return *this;
}

Vec3& Vec3::operator*=(const Vec3 &b) {
    x *= b.x;
    y *= b.y;
    z *= b.z;
    return *this;
}


Vec3& Vec3::operator/=(float b) {
    x /= b;
    y /= b;
    z /= b;
    return *this;
}

bool Vec3::operator==(const Vec3 &obj) const
{
    return x == obj.x && y == obj.y && z == obj.z;
}



Vec3 Vec3::operator+(const Vec3& other) const {
    return Vec3(x + other.x, y + other.y, z + other.z);
}

Vec3& Vec3::operator+=(const Vec3& other) {
    x += other.x;
    y += other.y;
    z += other.z;
    return *this;
}

Vec3 &Vec3::operator+=(const float &val)
{
    x += val;
    y += val;
    z += val;
    return *this;
}

Vec3 Vec3::operator-() const {
    return Vec3(-x, -y, -z);
}

Vec3 Vec3::operator-(const Vec3& other) const {
    return Vec3(x - other.x, y - other.y, z - other.z);
}

Vec3& Vec3::operator-=(const Vec3& other) {
    x -= other.x;
    y -= other.y;
    z -= other.z;
    return *this;
}


Vec3 Vec3::operator*(const Vec3& other) const {
    return Vec3(x * other.x, y * other.y, z * other.z);
}

float Vec3::operator[](const int &axes) const
{
    int r = axes % 3;
    if (r == 0) return x;
    else if (r == 1) return y;
    else return z;
}

Vec3 Vec3::normalize() const {
    float len = std::sqrt(x * x + y * y + z * z);
    return Vec3(x / len, y / len, z / len);
}

float Vec3::dot(const Vec3& other) const {
    return x * other.x + y * other.y + z * other.z;
}

Vec3 Vec3::randomUnitVector()
{
    static std::mt19937 gen(std::random_device{}());
    static std::uniform_real_distribution<float> dist(-1.0f, 1.0f);

    Vec3 p;
    do {
        p = Vec3(dist(gen), dist(gen), dist(gen));
    } while (p.lengthSquared() >= 1.0f);  // Повторяем, пока не получим вектор длиной меньше 1

    return p.normalize();  // Нормализуем вектор, чтобы его длина стала 1
}


Vec3 Vec3::cross(const Vec3& other) const {
    return Vec3(
        y * other.z - z * other.y,
        z * other.x - x * other.z,
        x * other.y - y * other.x
        );
}

float Vec3::length() const
{
    return std::sqrt(x * x + y * y + z * z);
}

float Vec3::lengthSquared() const
{
    return x * x + y * y + z * z;
}

Vec3 Vec3::getRandomDirection()
{
    static std::default_random_engine generator(std::random_device{}());
    std::uniform_real_distribution<float> distribution(-1.0f, 1.0f);

    Vec3 dir(distribution(generator), distribution(generator), distribution(generator));

    if (dir.length() == 0) {
        dir = Vec3(1.0f, 0.0f, 0.0f); // Задаем произвольное направление, если вектор нулевой
    }
    return dir.normalize();
}

Vec3 Vec3::sampleHemisphere(const Vec3 &normal)
{
    // Генерируем случайные значения u1 и u2
    float u1 = static_cast<float>(rand()) / RAND_MAX; // [0, 1)
    float u2 = static_cast<float>(rand()) / RAND_MAX; // [0, 1)

    // Преобразуем u1 в z-координату
    float z = u1; // z = cos(theta)
    float r = sqrt(1 - z * z); // r = sin(theta)

    // Преобразуем u2 в углы для x и y
    float theta = 2 * M_PI * u2; // Угол в радианах
    float x = r * cos(theta); // x = r * cos(theta)
    float y = r * sin(theta); // y = r * sin(theta)

    // Создаем вектор в пространстве
    Vec3 sample = Vec3(x, y, z);

    // Если нормаль не единичная, то нормализуем
    if (normal.length() != 0)
    {
        // Создаем ортонормированный базис
        Vec3 tangent = (fabs(normal.x) > 0.1f) ? Vec3(0, 1, 0) : Vec3(1, 0, 0);
        Vec3 bitangent = normal.cross(tangent).normalize();
        tangent = bitangent.cross(normal).normalize();

        // Преобразуем выборку в локальную систему координат
        return (tangent * sample.x + bitangent * sample.y + normal * sample.z).normalize();
    }

    return sample.normalize(); // Если нормаль нулевая, просто возвращаем выборку
}

Vec3 Vec3::rotateAroundAxis(const Vec3 &v, const Vec3 &axis, float angle)
{
    float cosA = std::cos(angle);
    float sinA = std::sin(angle);

    return v * cosA + axis.cross(v) * sinA + axis * axis.dot(v) * (1 - cosA);
}

Vec3 Vec3::reflect(const Vec3 &incident, const Vec3 &normal)
{
    return incident - normal * 2.0f * incident.dot(normal);
}

Vec3 Vec3::refract(const Vec3 &incident, const Vec3 &normal, float etai, float etat)
{
    float cosi = std::clamp(-1.0f, 1.0f, incident.dot(normal));
    Vec3 n = normal;

    if (cosi < 0)
    {
        cosi = -cosi;
    }
    else
    {
        std::swap(etai, etat);
        n = -normal;
    }

    float etaRatio = etai / etat;
    float k = 1 - etaRatio * etaRatio * (1 - cosi * cosi);

    if (k < 0)
    {
        return Vec3(0.0f, 0.0f, 0.0f);
    }
    else
    {
        return incident * etaRatio + n * (etaRatio * cosi - std::sqrt(k));
    }
}


Vec3 Vec3::refract(const Vec3& incident, const Vec3& normal, float eta)
{
    float cosi = std::clamp(-1.0f, 1.0f, incident.dot(normal));
    float etai = 1.0f;
    float etat = eta;
    Vec3 n = normal;

    // Если луч входит в материал, меняем направления и коэффициенты
    if (cosi < 0)
    {
        cosi = -cosi;
    }
    else
    {
        std::swap(etai, etat);
        n = -normal;
    }

    float etaRatio = etai / etat;
    float k = 1 - etaRatio * etaRatio * (1 - cosi * cosi);

    if (k < 0)
        return Vec3(0, 0, 0); // полное внутреннее отражение

    return incident * etaRatio + n * (etaRatio * cosi - std::sqrt(k));
}

float Vec3::fresnel(const Vec3 &incident, const Vec3 &normal, float ior)
{
    float cosi = std::clamp(incident.dot(normal), -1.0f, 1.0f);
    float etai = 1.0f, etat = ior;

    if (cosi > 0)
    {
        std::swap(etai, etat);
    }

    float sint = etai / etat * std::sqrt(std::max(0.0f, 1 - cosi * cosi));

    if (sint >= 1)
    {
        return 1.0f;
    }
    else
    {
        float cost = std::sqrt(std::max(0.0f, 1 - sint * sint));
        cosi = std::fabs(cosi);
        float Rs = ((etat * cosi) - (etai * cost)) / ((etat * cosi) + (etai * cost));
        float Rp = ((etai * cosi) - (etat * cost)) / ((etai * cosi) + (etat * cost));
        return (Rs * Rs + Rp * Rp) / 2.0f;
    }
}

Ray::Ray(const Vec3 &o, const Vec3 &d, bool inside, float refraction)
    : origin(o), direction(d), insideObject(inside), previousRefraction(refraction) {
}

GraphicParams::GraphicParams()
{
    _color = Vec3(1,1,1);
    _emission = {Vec3(0,0,0), 0};
    _reflectivity = 0;
    _refractiveIndex = 1;
    _transparency = 0;
}
