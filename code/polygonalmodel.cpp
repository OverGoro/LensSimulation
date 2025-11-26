#include "polygonalmodel.h"
#include "qdir.h"


Vec3 PolygonalModel::position() const
{
    return boundingSphereCenter;
}

void PolygonalModel::setPosition(const Vec3 &pos)
{
    Vec3 delta = pos - position();
    for (auto & p : polygons)
    {
        p.move(delta);
    }
    boundingSphereCenter += delta;
}

PolygonalModel::PolygonalModel(const std::vector<Polygon>& polys)
    : polygons(polys)
{
    calculateBoundingSphere();
}

bool PolygonalModel::intersect(const Ray &ray) const
{
    if (!intersectBoundingSphere(ray)) return false;

    bool hasHit = false;
    float closestT = std::numeric_limits<float>::max();

    for (const auto& poly : polygons) {
        float currentT;
        if (poly.intersect(ray, currentT) && currentT < closestT) {
            closestT = currentT;
            hasHit = true;
        }
    }
    return hasHit;
}

void PolygonalModel::calculateBoundingSphere() {
    Vec3 centerSum(0, 0, 0);
    int vertexCount = 0;

    for (const auto& poly : polygons) {
        centerSum = centerSum + poly.v0 + poly.v1 + poly.v2;
        vertexCount += 3;
    }

    boundingSphereCenter = centerSum * (1.0f / vertexCount);

    boundingSphereRadius = 0.0f;
    for (const auto& poly : polygons) {
        boundingSphereRadius = std::max(boundingSphereRadius, (poly.v0 - boundingSphereCenter).length());
        boundingSphereRadius = std::max(boundingSphereRadius, (poly.v1 - boundingSphereCenter).length());
        boundingSphereRadius = std::max(boundingSphereRadius, (poly.v2 - boundingSphereCenter).length());
    }
}

void PolygonalModel::setRefractionIndex(double refrIndex)
{
    for (auto &p : polygons)
        p._params._refractiveIndex = refrIndex;
}

void PolygonalModel::setTransparency(double transparency)
{
    for(auto &p : polygons)
        p._params._transparency = transparency;
}

bool PolygonalModel::intersectBoundingSphere(const Ray& ray) const {
    Vec3 oc = ray.origin - boundingSphereCenter;
    float half_b = oc.dot(ray.direction);
    float c = oc.dot(oc) - boundingSphereRadius * boundingSphereRadius;
    float discriminant = half_b * half_b - c;

    return discriminant >= 0;
}

void PolygonalModel::load(const QString &filename)
{
    polygons.clear(); // Очищаем текущие полигоны

    QFile file(filename);
    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        throw std::runtime_error("Unable to open file: " + filename.toStdString());
    }

    std::vector<Vec3> vertices; // Вектор для хранения вершин
    QTextStream in(&file);

    while (!in.atEnd()) {
        QString line = in.readLine().trimmed(); // Читаем строку и удаляем пробелы по краям
        if (line.isEmpty() || line.startsWith('#')) {
            continue; // Игнорируем пустые строки и комментарии
        }

        QStringList parts = line.split(' ', Qt::SkipEmptyParts); // Разбиваем строку на части
        if (parts.isEmpty()) {
            continue; // Игнорируем пустые части
        }

        if (parts[0] == "v") { // Вершина
            if (parts.size() < 4) {
                throw std::runtime_error("Invalid vertex format in line: " + line.toStdString());
            }
            Vec3 vertex;
            vertex.x = parts[1].toFloat();
            vertex.y = parts[2].toFloat();
            vertex.z = parts[3].toFloat();
            vertices.push_back(vertex); // Добавляем вершину в вектор
        }
        else if (parts[0] == "f") { // Полигон
            std::vector<int> faceIndices;
            for (int i = 1; i < parts.size(); ++i) {
                bool ok;
                int index = parts[i].split("/")[0].toInt(&ok);
                if (ok) {
                    //throw std::runtime_error("Invalid face vertex index in line: " + line.toStdString());
                    faceIndices.push_back(index - 1); // Индексы в OBJ начинаются с 1, а в векторе с 0
                }
            }

            // Убедимся, что у нас есть ровно три вершины для полигона
            if (faceIndices.size() >= 3) {
                // throw std::runtime_error("Invalid face format, must have 3 vertices in line: " + line.toStdString());
                // Создаем полигон и добавляем его в модель
                Polygon poly(vertices[faceIndices[0]], vertices[faceIndices[1]], vertices[faceIndices[2]],
                             _params._color, _params._transparency, _params._refractiveIndex, _params._reflectivity);
                addPolygon(poly);
            }

        }
    }

    file.close(); // Закрываем файл
}


void PolygonalModel::scale(double k)
{
    for (auto &p : polygons)
    {
        p.scale(position(), k);
    }
}

void PolygonalModel::rotate(const Vec3 &axis, double angle)
{
    for (auto &p : polygons)
        p.rotate(position(), axis, angle);
    calculateBoundingSphere();
}

bool PolygonalModel::intersect(const Ray& ray, float& t) const {
    if (!intersectBoundingSphere(ray)) return false;

    bool hasHit = false;
    float closestT = std::numeric_limits<float>::max();

    for (const auto& poly : polygons) {
        float currentT;
        if (poly.intersect(ray, currentT) && currentT < closestT) {
            closestT = currentT;
            hasHit = true;
        }
    }

    if (hasHit) {
        t = closestT;
    }
    return hasHit;
}

GraphicParams PolygonalModel::hitParams(const Ray &ray, float t) const
{
    float closestT = std::numeric_limits<float>::max();
    const Polygon *lastPoly = nullptr;
    for (const auto& poly : polygons) {
        float currentT;
        if (poly.intersect(ray, currentT) && currentT < closestT) {
            closestT = currentT;
            lastPoly = &poly;
        }
    }
    return lastPoly->hitParams(ray, t);
}

void PolygonalModel::addPolygon(const Polygon& poly) {
    polygons.push_back(poly);
    calculateBoundingSphere();
}
