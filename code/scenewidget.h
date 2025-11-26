#ifndef SCENEWIDGET_H
#define SCENEWIDGET_H

#include <QWidget>
#include "drawer.h"
#include "polygonalmodel.h"
#include "qlistwidget.h"
#include "scene.h"
#include "sphere.h"
#include "polygon.h"
#include "thinlens.h"

namespace Ui {
class SceneWidget;
}

class SceneWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SceneWidget(QWidget *parent = nullptr);
    ~SceneWidget();
public slots:
    void loadScene(const std::shared_ptr<Scene> &scene);
    void loadDrawer(const std::shared_ptr<Drawer> &drawer);
    void setParams();
protected slots:
    void onObjectSelected(QListWidgetItem *item);
signals:
    void renderSignal();
    void updateMapSignal();
    void applyChanges();
private:
    void clearParamsWidget();
    QWidget* createParamsWidgetForObject(const std::shared_ptr<BaseObject>& obj);
    QWidget* createSphereParamsWidget(const std::shared_ptr<Sphere>& sphere);
    QWidget* createPolygonParamsWidget(const std::shared_ptr<Polygon>& polygon);
    QWidget* createThinLensParamsWidget(const std::shared_ptr<Lens>& lens);
    QWidget *createModelParamsWidget(const std::shared_ptr<PolygonalModel>& lens);

private:
    Ui::SceneWidget *ui;
    std::shared_ptr<Scene> _scene;
    std::shared_ptr<Drawer> _drawer;
    QMap<QListWidgetItem*, std::shared_ptr<BaseObject>> _objectMap;
    bool _objsUpdated;



};

#endif // SCENEWIDGET_H
