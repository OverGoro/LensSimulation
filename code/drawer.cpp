#include "drawer.h"
#include <QOpenGLFunctions>
#include <QImage>
#include <QDebug>
#include <future>
#include <qalgorithms.h>
#include <QElapsedTimer>

Drawer::Drawer(RenderingWidget *widget, QObject *parrent) : QObject(parrent), _widget(widget)
{
    initialize();
    connect(this, &Drawer::progressChanged, _widget, &RenderingWidget::setProgress);
}

Drawer::~Drawer()
{

}

void Drawer::renderFrame(const std::shared_ptr<Scene> &scene)
{
    _frameProcessedRows = 0;
    _framebuffer.resize(_widget->getImageWidgetSize().height() * _widget->getImageWidgetSize().width());
    processPixels(0, _widget->getImageWidgetSize().height(), scene);
    _widget->setImage(_framebuffer, _widget->getImageWidgetSize().width(), _widget->getImageWidgetSize().height());
}

void Drawer::initialize()
{
    _framebuffer.resize(_widget->getImageWidgetSize().height() * _widget->getImageWidgetSize().width());
    for (auto &e : _framebuffer) e = Vec3(0,0,0);
    _widget->setImage(_framebuffer, _widget->getImageWidgetSize().width(), _widget->getImageWidgetSize().height());
}

int Drawer::getClosestNodes(const Ray &ray, const std::shared_ptr<Scene> &scene, const PhotonTree &photonMap, const PhotonTree &causticsMap, int depth)
{
    if (depth <= 0)
        return 0;

    float t_min = std::numeric_limits<float>::max();
    std::shared_ptr<BaseObject> hitObject = nullptr;
    GraphicParams hitParams;

    for (const auto &o : scene->objects())
    {
        float t = 0;
        if (o->intersect(ray, t) && t < t_min)
        {
            t_min = t;
            hitParams = o->hitParams(ray, t);
            hitObject = o;
        }
    }

    if (hitObject != nullptr)
    {
        Vec3 hitPoint = ray.origin + ray.direction * t_min;
        Vec3 bias = hitParams._normal * 1e-4f;
        Vec3 color(0.0f, 0.0f, 0.0f);

        int refractedPhotonsNum = 0;
        int reflectedPhotonsNum = 0;

        // Обработка преломлений
        if (hitParams._transparency > 0.0f)
        {
            Vec3 refractedDir = Vec3::refract(ray.direction, hitParams._normal, ray.previousRefraction, hitParams._refractiveIndex).normalize();
            Ray refractedRay(hitPoint + bias * ((ray.insideObject) ? -1.0f : 1.0f), refractedDir, !ray.insideObject, (ray.insideObject) ? hitParams._refractiveIndex : 1.0f);
            refractedPhotonsNum = getClosestNodes(refractedRay, scene, photonMap, causticsMap, depth - 1);
        }

        // Обработка отражений
        if (hitParams._reflectivity > 0.0f)
        {
            Vec3 reflectedDir = Vec3::reflect(ray.direction, hitParams._normal).normalize();
            Ray reflectedRay(hitPoint + bias, reflectedDir);
            refractedPhotonsNum = refractedPhotonsNum = getClosestNodes(reflectedRay, scene, photonMap, causticsMap, depth - 1);
        }



        std::vector<Photon> closestPhotons = photonMap.findPhotonsInRadius(hitPoint, _indirectLightMaxR);

        if (closestPhotons.size() > 0)
        {
            _avgDirectPhotnsNum += closestPhotons.size();
            _directPhotonsNum++;
        }

        std::vector<Photon> closestCaustics = causticsMap.findPhotonsInRadius(hitPoint, _indirectLightMaxR);


        return qMax((int)closestPhotons.size() + (int)closestCaustics.size(), qMax(refractedPhotonsNum, reflectedPhotonsNum));
    }
    return 0;
}



Vec3 Drawer::trace(const Ray &ray, const std::shared_ptr<Scene> &scene, const PhotonTree &photonMap, const PhotonTree &causticsMap, int depth)
{
    if (depth <= 0)
        return Vec3(0, 0, 0);

    float t_min = std::numeric_limits<float>::max();
    std::shared_ptr<BaseObject> hitObject = nullptr;
    GraphicParams hitParams;

    for (const auto &o : scene->objects())
    {
        float t = 0;
        if (o->intersect(ray, t) && t < t_min)
        {
            t_min = t;
            hitParams = o->hitParams(ray, t);
            hitObject = o;

        }
    }

    if (hitObject != nullptr)
    {
        Vec3 hitPoint = ray.origin + ray.direction * t_min;
        Vec3 viewDir = -ray.direction;
        Vec3 bias = hitParams._normal * 1e-4f;
        Vec3 color(0.0f, 0.0f, 0.0f);

        // // Прямой свет от точечных источников
        // int lightsNum = 0; // Количество освещающих источников
        // for (const auto &light : scene->lights())
        // {
        //     Vec3 lightDir = (light->position - hitPoint).normalize();
        //     float lightDistance = (light->position - hitPoint).length();

        //     // Проверка, не находится ли точка в тени
        //     Ray shadowRay(hitPoint + bias, lightDir);
        //     bool isInShadow = false;

        //     for (const auto &obj : scene->objects())
        //     {
        //         float t = 0;
        //         if (obj->intersect(shadowRay, t) && t < lightDistance)
        //         {
        //             isInShadow = true;
        //             break;
        //         }
        //     }

        //     // Если точка не в тени, добавляем прямое освещение
        //     if (!isInShadow)
        //     {
        //         lightsNum++;
        //         float intensity = std::max(0.0f, hitParams._normal.dot(lightDir));
        //         color += hitParams._color * intensity * light->color * (1.0f - hitParams._transparency);
        //     }
        // }

        // Прямое свечение от светящихся объектов

        // for (const auto &lightObj : scene->objects())
        // {
        //     // Пропускаем объекты без эмиссии
        //     if (lightObj->_params._emission.color == Vec3(0,0,0))
        //         continue;

        //     Vec3 lightDir = (lightObj->position() - hitPoint).normalize();
        //     float lightDistance = (lightObj->position() - hitPoint).length();

        //     // Проверка, не находится ли точка в тени
        //     Ray shadowRay(hitPoint + bias, lightDir);
        //     bool isInShadow = false;

        //     for (const auto &obj : scene->objects())
        //     {
        //         if (obj == lightObj || obj == hitObject) continue;
        //         float t = 0;
        //         if (obj->intersect(shadowRay, t) && t < lightDistance)
        //         {
        //             isInShadow = true;
        //             break;
        //         }
        //     }

        //     // Если точка не в тени, добавляем эмиссию от светящегося объекта
        //     if (!isInShadow)
        //     {
        //         float intensity = std::max(0.0f, hitParams._normal.dot(lightDir));
        //         color += hitParams._color * (1.0f - hitParams._reflectivity) * intensity * lightObj->_params._emission.color * lightObj->_params._emission.intensity * (1.0f - hitParams._transparency);
        //     }
        // }


        // Собственное свечение
        color += hitParams._emission.color * hitParams._emission.intensity;
        // color = hitParams._color;

        // Обработка преломлений
        if (hitParams._transparency > 0.0f)
        {
            Vec3 refractedDir = Vec3::refract(ray.direction, hitParams._normal, ray.previousRefraction, hitParams._refractiveIndex).normalize();
            Ray refractedRay(hitPoint + bias * ((ray.insideObject) ? -1.0f : 1.0f), refractedDir, !ray.insideObject, (ray.insideObject) ? hitParams._refractiveIndex : 1.0f);
            Vec3 refractedColor = trace(refractedRay, scene, photonMap, causticsMap, depth - 1) * hitParams._color;
            float fresnelCoeff = Vec3::fresnel(ray.direction, hitParams._normal, hitParams._refractiveIndex);
            Vec3 reflectedDir = Vec3::reflect(ray.direction, hitParams._normal).normalize();
            Ray reflectedRay(hitPoint + bias, reflectedDir);
            Vec3 reflectedColor = trace(reflectedRay, scene, photonMap, causticsMap, depth - 1) * hitParams._color;
            color += reflectedColor * fresnelCoeff + refractedColor * (1.0f - fresnelCoeff);
        }

        // Обработка отражений
        if (hitParams._reflectivity > 0.0f)
        {
            Vec3 reflectedDir = Vec3::reflect(ray.direction, hitParams._normal).normalize();
            Ray reflectedRay(hitPoint + bias, reflectedDir);
            Vec3 reflectedColor = trace(reflectedRay, scene,photonMap, causticsMap, depth - 1);
            Vec3 lightedColor = hitParams._color * (1.0f - hitParams._reflectivity) * reflectedColor * (1.0f - hitParams._transparency);
            reflectedColor = reflectedColor * hitParams._reflectivity * (1 - hitParams._transparency);
            color += lightedColor + reflectedColor;
        }



        std::vector<Photon> closestPhotons = causticsMap.findPhotonsInRadius(hitPoint, _indirectLightMaxR);
        double totalWeight = 0.0;
        Vec3 indirectColor = Vec3(0,0,0);
        for (const auto &closestNode : closestPhotons)
        {
            float intensity = std::max(0.0f, hitParams._normal.dot(-closestNode.direction));
            float distance = (hitPoint - closestNode.position).length();
            float w1 = std::max(0.0, 1 - distance / (_filterConstant * _indirectLightMaxR));
            totalWeight += w1;
            indirectColor += hitParams._color * intensity * w1 * closestNode.color * (1.0f - hitParams._transparency);
        }
        if (closestPhotons.size() > 0)
        {
            indirectColor /= totalWeight;
            indirectColor *= (double)closestPhotons.size() / _maxNearestPhotonsNum;
            // indirectColor /= _avgDirectPhotnsNum / _maxNearestPhotonsNum;
            // qDebug() << totalWeight << avgDensity << closestPhotons.size();
            // indirectColor *= w2;
            color += indirectColor;
        }


        closestPhotons = photonMap.findPhotonsInRadius(hitPoint, _indirectLightMaxR);
        totalWeight = 0.0;
        indirectColor = Vec3(0,0,0);
        for (const auto &closestNode : closestPhotons)
        {
            float intensity = std::max(0.0f, hitParams._normal.dot(-closestNode.direction));
            float distance = (hitPoint - closestNode.position).length();
            float w1 = std::max(0.0, 1 - distance / (_filterConstant * _indirectLightMaxR));
            totalWeight += w1;
            indirectColor += hitParams._color * intensity * w1 * closestNode.color * (1.0f - hitParams._transparency);
        }
        if (closestPhotons.size() > 0)
        {
            indirectColor /= totalWeight;
            indirectColor *= _avgDirectPhotnsNum / _maxNearestPhotonsNum;

            color += indirectColor;
        }

        // Нормализация цвета
        color.x = std::min(color.x, 1.0f);
        color.y = std::min(color.y, 1.0f);
        color.z = std::min(color.z, 1.0f);
        return color;
    }

    return gi.color;
}

void Drawer::processPixels(int startRow, int endRow, const std::shared_ptr<Scene> &scene)
{

    QElapsedTimer timer;
    timer.start();

    double width = _widget->getImageWidgetSize().width();
    double height = _widget->getImageWidgetSize().height();
    double aspectRatio = width / height;

    Vec3 cameraPos = scene->camera()->position();
    Vec3 cameraDir = scene->camera()->direction().normalize();
    double screenDistance = scene->camera()->screenDistance();
    double fov = scene->camera()->fov();
    double tanFov = tan(fov * M_PI / 360.0f);  // Тангенс половины угла обзора

    // Базовые оси камеры (по умолчанию)
    Vec3 right = Vec3(1, 0, 0).cross(cameraDir).normalize();
    Vec3 up = cameraDir.cross(right).normalize();

    // Вектор для хранения будущих задач
    std::vector<std::future<void>> futures;

    _maxNearestPhotonsNum = 1;
    _avgDirectPhotnsNum = 0;
    _directPhotonsNum = 0;

    double sumNearestPhotonsNum = 0;

    for (int j = startRow; j < endRow; ++j)
    {
        for (int i = 0; i < width; ++i)
        {
            // Преобразование пиксельных координат в мировые
            float x = (2.0f * (i + 0.5f) / float(width) - 1.0f) * aspectRatio * tanFov;
            float y = (1.0f - 2.0f * (j + 0.5f) / float(height)) * tanFov;

            // Позиция на виртуальном экране
            Vec3 pixelOnScreen = cameraPos + cameraDir * screenDistance + right * x + up * y;
            Vec3 rayDirection = (pixelOnScreen - cameraPos).normalize();

            Ray ray(cameraPos, rayDirection);
            int v = getClosestNodes(ray, scene, scene->photonMap(), scene->causticsPhotonMap(), _renderingDepth);
            _maxNearestPhotonsNum = qMax(_maxNearestPhotonsNum, v);
            sumNearestPhotonsNum += v;
        }
    }
    if (_directPhotonsNum > 0)
        _avgDirectPhotnsNum /= _directPhotonsNum;
    _maxNearestPhotonsNum = sumNearestPhotonsNum / width / height;
    // Запускаем обработку строк в параллельных задачах
    for (int j = startRow; j < endRow; ++j)
    {
        // Создаем задачу для обработки строки
        futures.push_back(std::async(std::launch::async, [this, j, width, height, cameraPos, cameraDir, screenDistance, right, up, aspectRatio, &scene, tanFov]() {
            for (int i = 0; i < width; ++i)
            {
                // Преобразование пиксельных координат в мировые
                float x = (2.0f * (i + 0.5f) / float(width) - 1.0f) * aspectRatio * tanFov;
                float y = (1.0f - 2.0f * (j + 0.5f) / float(height)) * tanFov;

                // Позиция на виртуальном экране
                Vec3 pixelOnScreen = cameraPos + cameraDir * screenDistance + right * x + up * y;
                Vec3 rayDirection = (pixelOnScreen - cameraPos).normalize();

                Ray ray(cameraPos, rayDirection);
                _framebuffer[j * width + i] = trace(ray, scene, scene->photonMap(), scene->causticsPhotonMap(), _renderingDepth);
            }

            // Обновление прогресса
            int completedRows = ++_frameProcessedRows;
            emit progressNameChanged("Создание изображения");
            emit progressChanged(((double)(completedRows) / height) * 100.0f);
        }));
    }

    // Ждем завершения всех задач
    for (auto &future : futures)
    {
        future.get();
    }

    // qDebug() << "Фотонов на источник:" << _photonsPerLight  << "Время отрисовки:" << timer.elapsed() / 1000.0 << "c";
}

float Drawer::indirectLightMaxR() const
{
    return _indirectLightMaxR;
}

void Drawer::setIndirectLightMaxR(float newIndirectLightMaxR)
{
    _indirectLightMaxR = newIndirectLightMaxR;
}

double Drawer::filterConstant() const
{
    return _filterConstant;
}

void Drawer::setFilterConstant(double newFilterConstant)
{
    _filterConstant = newFilterConstant;
}


void Drawer::updatePhotonMap(const std::shared_ptr<Scene> &scene)
{
    QElapsedTimer timer;
    timer.start();
    // std::vector<photon> photons;
    std::vector<Photon> photons;
    std::vector<Photon> causticPhotons;
    scene->updatePhotonMap(photons, causticPhotons,_nearestPhotonsNum);
    double totalPhotons = _photonsPerLight * scene->lights().size();
    double processedPhotons = 0;

    for (const auto& o: scene->objects()) if(o->_params._emission.intensity > 0) totalPhotons += _photonsPerLight;

    emit progressNameChanged("Рассчет фотонной карты");

    for (const auto &light : scene->objects())
    {
        if (light->_params._emission.color == Vec3(0, 0, 0))
            continue;

        for (int i = 0; i < _photonsPerLight; ++i)
        {
            Vec3 randomDir = Vec3::getRandomDirection();
            Vec3 colorMasks[3] = {Vec3(1,0,0), Vec3(0,1,0), Vec3(0,0,1)};
            Photon photon;
            photon.position = light->position();
            photon.direction = randomDir;

            // Проверка пересечения с объектами
            bool intersectsObject = false;

            for (const auto &object : scene->objects())
            {
                float t;
                Ray r(photon.position, photon.direction);
                if (object == light && object->intersect(r, t))
                {
                    Vec3 bias = object->hitParams(r, t)._normal * 1e-4f;
                    photon.position = r.origin + r.direction * t + bias;
                    break;
                }
            }
            for (const auto &object : scene->objects())
            {
                float t;
                if (object != light && object->intersect(Ray(photon.position, photon.direction), t))
                {
                    intersectsObject = true;
                    break;
                }
            }


            auto prevSize = photons.size();
            for (int j = 0; j < 3 && intersectsObject; j++)
            {
                Photon tmpPhoton = photon;
                tmpPhoton.color = light->_params._emission.color * light->_params._emission.intensity * colorMasks[j];
                tracePhoton(tmpPhoton, scene->objects(), photons, causticPhotons, _renderingDepth, 1, _renderingDepth);
            }
            if (photons.size() > prevSize)
                emit progressChanged((++processedPhotons) / totalPhotons * 95);
            else
                --i;
        }
    }
    // qDebug() << "Photons NUm: " << photons.size();
    scene->updatePhotonMap(photons, causticPhotons, _nearestPhotonsNum);
    emit progressChanged((++processedPhotons) / totalPhotons * 100);
    //qDebug() << "Фотонов на источник:" << _photonsPerLight  << "Время построения карты:" << timer.elapsed() / 1000.0 << "c";
}

int Drawer::nearestPhotonsNum() const
{
    return _nearestPhotonsNum;
}

void Drawer::setNearestPhotonsNum(int newNearestPhotonsNum)
{
    _nearestPhotonsNum = newNearestPhotonsNum;
}

int Drawer::photonsPerLight() const
{
    return _photonsPerLight;
}

void Drawer::setPhotonsPerLight(int newPhotonsPerLight)
{
    _photonsPerLight = newPhotonsPerLight;
}

int Drawer::renderingDepth() const
{
    return _renderingDepth;
}

void Drawer::setRenderingDepth(int newRenderingDepth)
{
    _renderingDepth = newRenderingDepth;
}
