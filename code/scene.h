#ifndef SCENE_H
#define SCENE_H

#include "baseobject.h"
#include "light.h"
#include "camera.h"
#include "photon.h"
#include <memory>
#include <vector>
#include <QObject>


class Scene : public QObject
{
    Q_OBJECT
public:
    Scene();
    void addObject(const std::shared_ptr<BaseObject> &obj);
    void addLight(const std::shared_ptr<Light> light);
    std::vector<std::shared_ptr<BaseObject> > objects() const;

    std::vector<std::shared_ptr<Light> > lights() const;

    std::shared_ptr<Camera> camera() const;
    void setCamera(const std::shared_ptr<Camera> &newCamera);

    const PhotonTree &photonMap() const;
    void updatePhotonMap(const std::vector<Photon> &photons, const std::vector<Photon> &causticsPhotons, int k);
    void setCausticsPhotonMap(const PhotonTree &newCausticsPhotonMap);

    const PhotonTree &causticsPhotonMap() const;

signals:
    void progressNameChanged(const QString &);
    void progressChanged(const double &);


protected:
    std::vector<std::shared_ptr<BaseObject>> _objects;
    std::vector<std::shared_ptr<Light>> _lights;
    std::shared_ptr<Camera> _camera;
    PhotonTree _photonMap;
    PhotonTree _causticsPhotonMap;
};

#endif // SCENE_H
