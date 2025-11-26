#ifndef DRAWMANAGER_H
#define DRAWMANAGER_H

#include <QObject>
#include <memory>
#include "scene.h"
#include "baseobject.h"
#include "drawer.h"

class DrawManager : public QObject
{
    Q_OBJECT
public:
    explicit DrawManager(QObject *parent = nullptr) = delete;
    DrawManager(const std::shared_ptr<Drawer> &drawer, QObject *parrent = nullptr);
    std::shared_ptr<Drawer> drawer() const;
    void setDrawer(const std::shared_ptr<Drawer> &newDrawer);
    void drawScene(const std::shared_ptr<Scene> scene);
    void updateSceneMap(const std::shared_ptr<Scene> scene);
signals:
protected:
    std::shared_ptr<Drawer> _drawer;
};

#endif // DRAWMANAGER_H
