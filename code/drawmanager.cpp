#include "drawmanager.h"

DrawManager::DrawManager(const std::shared_ptr<Drawer> &drawer, QObject *parrent)
    :QObject(parrent)
{
    setDrawer(drawer);
}

std::shared_ptr<Drawer> DrawManager::drawer() const
{
    return _drawer;
}

void DrawManager::setDrawer(const std::shared_ptr<Drawer> &newDrawer)
{
    _drawer = newDrawer;
}

void DrawManager::drawScene(const std::shared_ptr<Scene> scene)
{
    if (_drawer)
    {
        _drawer->renderFrame(scene);
    }
}

void DrawManager::updateSceneMap(const std::shared_ptr<Scene> scene)
{
    if (_drawer)
    {
        connect(scene.get(), &Scene::progressChanged, _drawer.get(),&Drawer::progressChanged);
        _drawer->updatePhotonMap(scene);
        disconnect(scene.get(), &Scene::progressChanged, _drawer.get(), &Drawer::progressChanged);
    }
}
