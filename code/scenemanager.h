#ifndef SCENEMANAGER_H
#define SCENEMANAGER_H

#include <QObject>
#include "scene.h"
#include "baseobject.h"
#include "scenewidget.h"
#include "photon.h"
#include <memory>

class SceneManager : public QObject
{
    Q_OBJECT
public:
    explicit SceneManager(QObject *parrent = nullptr) = delete;
    explicit SceneManager(SceneWidget *wgt, QObject *parent = nullptr);
    void addObject(const std::shared_ptr<BaseObject> &obj);
    void setCamera(const std::shared_ptr<Camera> &camera);
    void setObjects(const std::vector<std::shared_ptr<BaseObject>> &objs);
    std::shared_ptr<Scene> getScene();
    void setScene(const std::shared_ptr<Scene> &newScene);

signals:
    void paramsChanged(const std::shared_ptr<Scene> &scene);
protected:
    std::shared_ptr<Scene> _scene;
    std::shared_ptr<PhotonTree> _tree;
    SceneWidget *_widget;
};

#endif // SCENEMANAGER_H
