#include "scene.h"

Scene::Scene() {
    updatePhotonMap({}, {}, 1);
}

void Scene::addObject(const std::shared_ptr<BaseObject> &obj)
{
    _objects.push_back(obj);
}

void Scene::addLight(const std::shared_ptr<Light> light)
{
    _lights.push_back(light);
}

std::vector<std::shared_ptr<BaseObject> > Scene::objects() const
{
    return _objects;
}

std::vector<std::shared_ptr<Light> > Scene::lights() const
{
    return _lights;
}

std::shared_ptr<Camera> Scene::camera() const
{
    return _camera;
}

void Scene::setCamera(const std::shared_ptr<Camera> &newCamera)
{
    _camera = newCamera;
}

const PhotonTree &Scene::photonMap() const
{
    return _photonMap;
}

void Scene::updatePhotonMap(const std::vector<Photon> &photons, const std::vector<Photon> &causticsPhotons, int k)
{
    _photonMap = PhotonTree(photons, k);
    _causticsPhotonMap = PhotonTree(causticsPhotons, k);
}

void Scene::setCausticsPhotonMap(const PhotonTree &newCausticsPhotonMap)
{
    _causticsPhotonMap = newCausticsPhotonMap;
}

const PhotonTree &Scene::causticsPhotonMap() const
{
    return _causticsPhotonMap;
}
