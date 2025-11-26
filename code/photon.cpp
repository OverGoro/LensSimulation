#include "photon.h"
#include <iostream>
#include <random>
#include <limits>
#include <thread>
#include <future>



// Вспомогательная функция для вычисления расстояния
static float squaredDistance(const Vec3 &a, const Vec3 &b)
{
    Vec3 diff = a - b;
    return diff.lengthSquared();
}

// Основная функция поиска
void PhotonTree::findKClosestNodes(
    std::shared_ptr<PhotonNode> node,  // Текущий узел
    const Vec3 &point, // Целевая точка
    int K,             // Число ближайших
    std::priority_queue<
        std::pair<float, std::shared_ptr<PhotonNode> >, // Максимальная куча
        std::vector<std::pair<float, std::shared_ptr<PhotonNode> >>,
        Comparator> &maxHeap, // Куча ближайших узлов
    int depth                 // Глубина в дереве
    ) const
{
    if (!node)
        return; // Пустой узел

    float distSq = squaredDistance(node->photon.position, point);

    if ((int)maxHeap.size() < K)
    {
        maxHeap.emplace(distSq, node);
    }
    else if (distSq < maxHeap.top().first)
    {
        maxHeap.pop();
        maxHeap.emplace(distSq, node);
    }

    int axis = depth % 3;

    std::shared_ptr<PhotonNode> nearChild = (point[axis] < node->photon.position[axis]) ? node->left : node->right;
    std::shared_ptr<PhotonNode> farChild = (point[axis] < node->photon.position[axis]) ? node->right : node->left;

    findKClosestNodes(nearChild, point, K, maxHeap, depth + 1);

    float axisDist = fabs(point[axis] - node->photon.position[axis]);
    if ((int)maxHeap.size() < K || axisDist * axisDist < maxHeap.top().first)
    {
        findKClosestNodes(farChild, point, K, maxHeap, depth + 1);
    }
}

PhotonTree::PhotonTree()
{
    root = nullptr;
    K = 0;
}

PhotonTree::PhotonTree(const std::vector<Photon> &photonsSrc, size_t K) : K(K)
{
    root = nullptr;
    std::vector<Photon> photons = photonsSrc;
    root = buildTree(photons);

    // std::vector<std::shared_ptr<PhotonNode> > nodes;
    // std::function<void(std::shared_ptr<PhotonNode> )> collectNodes = [&](std::shared_ptr<PhotonNode> node)
    // {
    //     if (node)
    //     {
    //         nodes.push_back(node);
    //         collectNodes(node->left);
    //         collectNodes(node->right);
    //     }
    // };
    // collectNodes(root);

    std::cout << "Tree construction completed.\n";
}

std::shared_ptr<PhotonNode> PhotonTree::nearestPhotonNode(const Vec3 &point) const
{
    std::shared_ptr<PhotonNode> bestNode = nullptr;
    float bestDist = std::numeric_limits<float>::max();
    nearestPhotonNodeRecursion(root, point, 0, bestNode, bestDist);
    return bestNode;
}

void PhotonTree::nearestPhotonNodeRecursion(std::shared_ptr<PhotonNode> node, const Vec3 &point, int depth, std::shared_ptr<PhotonNode> &bestNode, float &bestDist) const
{
    if (!node)
        return;

    float dist = (node->photon.position - point).length();
    if (dist < bestDist)
    {
        bestDist = dist;
        bestNode = node;
    }

    int axis = depth % 3;
    std::shared_ptr<PhotonNode> nextNode = point[axis] < node->photon.position[axis] ? node->left : node->right;
    std::shared_ptr<PhotonNode> otherNode = nextNode == node->left ? node->right : node->left;

    nearestPhotonNodeRecursion(nextNode, point, depth + 1, bestNode, bestDist);
    if (fabs(point[axis] - node->photon.position[axis]) < bestDist)
    {
        nearestPhotonNodeRecursion(otherNode, point, depth + 1, bestNode, bestDist);
    }
}

void tracePhoton(Photon &photon, const std::vector<std::shared_ptr<BaseObject>> &objects,
                 std::vector<Photon> &photons,std::vector<Photon> &causticPhotons, int depth, float currentRefractiveIndex, int maxDepth)
{
    if (depth <= 0 || photon.color == Vec3(0,0,0))
        return;

    float t_min = std::numeric_limits<float>::max();
    std::shared_ptr<BaseObject> hitObject = nullptr;
    GraphicParams hitParams;

    // Поиск пересечения фотона с объектами
    for (const auto &o : objects)
    {
        float t = 0;
        if (o->intersect(Ray(photon.position, photon.direction), t) && t < t_min)
        {
            t_min = t;
            hitParams = o->hitParams(Ray(photon.position, photon.direction), t);
            hitObject = o;
        }
    }

    if (hitObject != nullptr)
    {
        photon.position += photon.direction * t_min; // Обновляем положение фотона
        if (hitParams._transparency < 1  && hitParams._reflectivity < 1)
        {
            if (depth == maxDepth)
                photons.push_back(photon); // Сохраняем фотон в карту общего освещения, если объект не прозрачный
            else
                causticPhotons.push_back(photon); // Сохраяняем в карту каустиков, если фотон уже был преломлен
        }
        // Показатель преломления объекта
        float objectRefractiveIndex = hitParams._refractiveIndex;

        // Отражение фотона
        if (hitParams._reflectivity > 0.0f)
        {
            Vec3 reflectedDir = Vec3::reflect(photon.direction, hitParams._normal).normalize();
            Photon reflectedPhoton = photon;
            reflectedPhoton.direction = reflectedDir;
            reflectedPhoton.color *= hitParams._reflectivity;
            tracePhoton(reflectedPhoton, objects, photons,causticPhotons, depth - 1, currentRefractiveIndex);
        }

        // Преломление фотона
        if (hitParams._transparency > 0.0f)
        {
            // Vec3 refractedDir = Vec3::refract(photon.direction, hitParams._normal, currentRefractiveIndex, hitParams._refractiveIndex);
            // Photon refractedPhoton = photon;
            // refractedPhoton.direction = refractedDir;
            // refractedPhoton.color = photon.color;
            // tracePhoton(refractedPhoton, objects, photonMap, depth - 1, hitParams._refractiveIndex, maxDepth);
            photon.color *= (hitParams._transparency);
            photon.color *= hitParams._color;
            float refractiveIndices[3];
            refractiveIndices[0] = objectRefractiveIndex + hitParams._refractionDeltaR;
            refractiveIndices[1] = objectRefractiveIndex;
            refractiveIndices[2] = objectRefractiveIndex + hitParams._refractionDeltaB;

            // Разделяем цвет на три компоненты (RGB)
            Vec3 colors[3] = {photon.color * Vec3(1, 0, 0), photon.color * Vec3(0, 1, 0), photon.color * Vec3(0, 0, 1)};

            for (int i = 0; i < 3; ++i)
            {
                Vec3 refractedDir = Vec3::refract(photon.direction, hitParams._normal, currentRefractiveIndex, refractiveIndices[i]);
                Photon refractedPhoton = photon;
                refractedPhoton.direction = refractedDir;
                refractedPhoton.color = colors[i];
                tracePhoton(refractedPhoton, objects, photons, causticPhotons, depth - 1, refractiveIndices[i], maxDepth);
            }
        }
    }
}

std::shared_ptr<PhotonNode> PhotonTree::buildTree(std::vector<Photon> &photons, int depth)
{
    if (photons.empty())
        return nullptr;

    int axis = depth % 3; // Разбиение по оси X, Y или Z
    std::sort(photons.begin(), photons.end(), [axis](const Photon &a, const Photon &b)
              { return a.position[axis] < b.position[axis]; });

    size_t median = photons.size() / 2;
    std::shared_ptr<PhotonNode> node = std::make_shared<PhotonNode>(photons[median]);

    if (photons.size() == 1)
    {
        // Если остался один фотон, завершаем дерево.
        node->left = nullptr;
        node->right = nullptr;
    }
    else
    {
        // Разделяем список фотонов на левую и правую половины
        std::vector<Photon> left(photons.begin(), photons.begin() + median);
        std::vector<Photon> right(photons.begin() + median + 1, photons.end());

        // Рекурсивное построение дерева
        node->left = buildTree(left, depth + 1);
        node->right = buildTree(right, depth + 1);
    }

    return node;
}

std::vector<std::shared_ptr<PhotonNode> > PhotonTree::getKClosestPhotons(std::shared_ptr<PhotonNode>  root, const Vec3& point, int K) const {
    Comparator cmp(point);
    std::priority_queue<
        std::pair<float, std::shared_ptr<PhotonNode> >,
        std::vector<std::pair<float, std::shared_ptr<PhotonNode> >>,
        Comparator
        > maxHeap(cmp);

    findKClosestNodes(root, point, K, maxHeap, 0); // depth = 0

    // Преобразуем кучу в вектор
    std::vector<std::shared_ptr<PhotonNode> > result;
    while (!maxHeap.empty()) {
        result.push_back(maxHeap.top().second);
        maxHeap.pop();
    }

    return result;
}


std::vector<Photon> PhotonTree::findPhotonsInRadius(const Vec3 &point, float radius, int maxNum) const
{
    std::vector<Photon> foundPhotons;
    float radiusSq = radius * radius; // Квадрат радиуса для сравнения

    std::function<void(std::shared_ptr<PhotonNode> , int)> search = [&](std::shared_ptr<PhotonNode> node, int depth) {
        if (!node) return;

        // Вычисляем квадрат расстояния до текущего фотона
        float distanceSq = squaredDistance(node->photon.position, point);
        if (distanceSq <= radiusSq && (maxNum == -1 || foundPhotons.size() < maxNum))
        {
            foundPhotons.push_back(node->photon);
        }

        // Определяем текущую ось
        int axis = depth % 3;

        // Проверяем, нужно ли искать в левом поддереве
        if ((maxNum == -1 || foundPhotons.size() < maxNum) && point[axis] - radius <= node->photon.position[axis])
        {
            search(node->left, depth + 1);
        }

        // Проверяем, нужно ли искать в правом поддереве
        if ((maxNum == -1 || foundPhotons.size() < maxNum) && point[axis] + radius >= node->photon.position[axis])
        {
            search(node->right, depth + 1);
        }
    };

    search(root, 0); // Начинаем с корня и глубины 0
    return foundPhotons;
}


Photon::Photon(const Vec3 &p, const Vec3 &dir, const Vec3 &col)
    : position(p), direction(dir), color(col){}


PhotonNode::PhotonNode(const Photon &p) : photon(p), left(nullptr), right(nullptr) {}

bool Comparator::operator()(std::pair<float, std::shared_ptr<PhotonNode> > a, std::pair<float, std::shared_ptr<PhotonNode> > b)
{
    return a.first < b.first; // Максимальная куча
}
