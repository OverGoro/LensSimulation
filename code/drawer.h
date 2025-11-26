#ifndef DRAWER_H
#define DRAWER_H

#include <mutex>
#include <vector>
#include <QObject>
#include "light.h"
#include "photon.h"
#include "renderingwidget.h"
#include "scene.h"

class Drawer : public QObject
{
    Q_OBJECT
public:
    Drawer(QObject *parrent) = delete;
    Drawer(RenderingWidget *widget, QObject *parrent);
    ~Drawer();
    int renderingDepth() const;
    void setRenderingDepth(int newRenderingDepth);

    int photonsPerLight() const;
    void setPhotonsPerLight(int newPhotonsPerLight);

    int nearestPhotonsNum() const;
    void setNearestPhotonsNum(int newNearestPhotonsNum);

    double filterConstant() const;
    void setFilterConstant(double newFilterConstant);

    float indirectLightMaxR() const;
    void setIndirectLightMaxR(float newIndirectLightMaxR);

public slots:
    void renderFrame(const std::shared_ptr<Scene> &_scene);
    void updatePhotonMap(const std::shared_ptr<Scene> &scene);
private:
    void initialize();

    // Vec3 trace (const Ray &ray, const std::shared_ptr<Scene> &scene, int depth);
    int getClosestNodes(const Ray &ray, const std::shared_ptr<Scene> &scene, const PhotonTree &photonMap, const PhotonTree &causticsMap, int depth);
    // double getLoghtFilter(const Ray &ray, const std::shared_ptr<Scene> &scene, const PhotonTree &photonMap, const PhotonTree &causticsMap);
    Vec3 trace (const Ray &ray, const std::shared_ptr<Scene> &scene, const PhotonTree &photonMap, const PhotonTree &causticsMap, int depth);
    // Vec3 trace(const Ray &ray, const std::shared_ptr<Scene> &scene, int depth);

    void processPixels(int startRow, int endRow, const std::shared_ptr<Scene> &scene);
    // void processPixels(int startRow, int endRow, const std::shared_ptr<Scene> &scene);


signals:
    void progressNameChanged(const QString &name);
    void progressChanged(double progress);


private:
    std::vector<Vec3> _framebuffer;

    std::atomic<int> _frameProcessedRows;
    std::mutex _processOutputMutex;

    RenderingWidget *_widget;

    int _renderingDepth = 10;
    int _photonsPerLight = 100000;
    int _nearestPhotonsNum = 50;
    int _maxNearestPhotonsNum = 50;
    double _avgDirectPhotnsNum = 1;
    int _directPhotonsNum = 1;
    float _indirectLightMaxR = 0.1;
    double _filterConstant = 1;


    const LightColor gi {
        Vec3(0, 0, 0),
        0
    };


};


#endif // DRAWER_H
