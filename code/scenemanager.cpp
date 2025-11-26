#include "scenemanager.h"

SceneManager::SceneManager(SceneWidget *wgt, QObject *parent)
    : QObject{parent}, _widget(wgt)
{
    _scene = std::make_shared<Scene>();
    connect(this, &SceneManager::paramsChanged, _widget, &SceneWidget::loadScene);
}

void SceneManager::addObject(const std::shared_ptr<BaseObject> &obj)
{
    _scene->addObject(obj);
    emit paramsChanged(_scene);
}

void SceneManager::setCamera(const std::shared_ptr<Camera> &camera)
{
    _scene->setCamera(camera);
    emit paramsChanged(_scene);
}

std::shared_ptr<Scene> SceneManager::getScene()
{
    return _scene;
}

void SceneManager::setScene(const std::shared_ptr<Scene> &newScene)
{
    _scene = newScene;
    emit paramsChanged(_scene);
}
