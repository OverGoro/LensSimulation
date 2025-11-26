#ifndef __PHOTON_H__
#define __PHOTON_H__

#include "primitives.h"
#include "light.h"
#include "baseobject.h"
#include <memory>
#include <vector>
#include <queue>
#include <functional>
#include <algorithm>

struct Photon
{
    Vec3 position;
    Vec3 direction;
    Vec3 color;

    Photon() : position(0, 0, 0), direction(0, 0, 0), color(0, 0, 0){}

    Photon(const Vec3 &p, const Vec3 &dir, const Vec3 &col);

    Photon (const Photon &other) = default;
    Photon &operator=(const Photon &other) = default;
    Photon (Photon &&other) = default;
    Photon &operator=(Photon &&other) = default;
};

// Узел KD-дерева
struct PhotonNode
{
    Photon photon;
    std::shared_ptr<PhotonNode> left, right;

    PhotonNode(const Photon &p);
};

struct Comparator
{
    const Vec3 &point;
    Comparator(const Vec3 &p) : point(p) {}

    bool operator()(std::pair<float, std::shared_ptr<PhotonNode> > a, std::pair<float, std::shared_ptr<PhotonNode> > b);
};


// KD-дерево для поиска ближайших фотонов
class PhotonTree
{
public:
    PhotonTree();
    ~PhotonTree() = default;
    PhotonTree(const std::vector<Photon> &photons, size_t K);
    std::shared_ptr<PhotonNode> nearestPhotonNode(const Vec3 &point) const;

public:
    std::shared_ptr<PhotonNode> buildTree(std::vector<Photon> &photons, int depth = 0);

    std::vector<std::shared_ptr<PhotonNode> > getKClosestPhotons(std::shared_ptr<PhotonNode>  root, const Vec3& point, int K) const;
    void findKClosestNodes(std::shared_ptr<PhotonNode> node,  // Текущий узел
                           const Vec3 &point, // Целевая точка
                           int K,             // Число ближайших
                           std::priority_queue<
                               std::pair<float, std::shared_ptr<PhotonNode> >, // Максимальная куча
                               std::vector<std::pair<float, std::shared_ptr<PhotonNode> >>,
                               Comparator> &maxHeap, // Куча ближайших узлов
                           int depth = 0             // Глубина в дереве
                           ) const;
    void buildClosestNodes(PhotonNode &node, const std::vector<std::shared_ptr<PhotonNode> > &nodes, size_t K);
    std::vector<Photon> findPhotonsInRadius(const Vec3 &point, float radius, int maxNum = -1) const;


    void nearestPhotonNodeRecursion(std::shared_ptr<PhotonNode> node, const Vec3 &point, int depth, std::shared_ptr<PhotonNode> &bestNode, float &bestDist) const;

    std::shared_ptr<PhotonNode> root;
    size_t K;
};


void tracePhoton(Photon &photon, const std::vector<std::shared_ptr<BaseObject>> &objects,
                 std::vector<Photon> &photonMap,std::vector<Photon> &causticPhotons, int depth = 15, float currentRefractiveIndex = 1, int maxDepth = 15);

#endif // __PHOTON_H__
