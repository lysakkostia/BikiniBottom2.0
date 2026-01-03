#include "Map.h"
#include "RandomGenerator.h"
#include "GameConstants.h"
#include "PerlinNoise.h"
#include <QFile>
#include <QDataStream>
#include <QIODevice>
#include <QString>
#include <QDebug>
#include <QDateTime>
#include <queue>
#include <unordered_map>
#include <functional>
#include <algorithm>

namespace Const_MZones = GlobalConst::MapGeneration::Zones;
namespace Const_METypes = GlobalConst::MapGeneration::EnemyTypes;
namespace GarantUnit = GlobalConst::MapGeneration::GarantUnitProbs;
namespace Noise = GlobalConst::NoiseSettings;

int HexMap::getRadius() const { return radiusInner; }
const std::vector<std::vector<Hex>>& HexMap::getMap() const { return mapGrid; }


HexMap::HexMap(int radius) : radiusInner(radius), enemyCounter(0)
{
    mapSeed = static_cast<unsigned int>(QDateTime::currentMSecsSinceEpoch());

    for(int q = -radius; q <=radius; q++)
    {
        int r1 = std::max(-radius, -q - radius);
        int r2 = std::min(radius, -q + radius);
        std::vector<Hex> Column;
        for(int r = r1; r <= r2; r++)
        {
            Column.emplace_back(q,r);
        }
        mapGrid.push_back(Column);
    }
    generateUnits();

    QPoint S = {0, 0};
    updateVisibility(S);
}

void HexMap::placeGuaranteedCampfire()
{
    std::vector<QPoint> Neighbors = {{1,0}, {1,-1}, {0,-1}, {-1,0}, {-1,1}, {0,1}};
    std::vector<QPoint> ValidPositions;

    for(const auto& offset : Neighbors)
    {
        if(containsHex(offset.x(), offset.y()))
        {
            ValidPositions.push_back(offset);
        }
    }

    if(!ValidPositions.empty())
    {
        int idx = RandGenerator::randIntInInterval(0, ValidPositions.size() - 1);
        QPoint pos = ValidPositions[idx];

        Hex& hex = getChangeableQPointLoc(pos);
        Unit* campfire = unitFabric.create(UnitType::CampfireUnit, 1, pos);
        if(campfire)
        {
            hex.setUnit(campfire);
        }
    }
}

int HexMap::calculateUnitLevel(int q, int r, bool isDangerZone) const
{
    int dist = getHexDistance(q, r);

    if (dist <= Const_MZones::SAFE_RADIUS) {
        return 1;
    }

    double effectiveDist = static_cast<double>(dist - Const_MZones::SAFE_RADIUS);
    double calculatedLevel = 1.0 + (effectiveDist * Const_MZones::LEVELS_PER_HEX);

    if (isDangerZone) {
        calculatedLevel = (calculatedLevel * Const_MZones::DANGER_ZONE_MULT) + Const_MZones::DANGER_ZONE_FLAT_ADD;
    }

    int variance = RandGenerator::randIntInInterval(Const_MZones::RAND_VAR_MIN, Const_MZones::RAND_VAR_MAX);
    int finalLevel = static_cast<int>(calculatedLevel) + variance;

    if (finalLevel < 1) finalLevel = 1;

    return finalLevel;
}

UnitType HexMap::chooseRandomEnemyType() const
{
    double roll = RandGenerator::randDoubleInInterval(0.0, 1.0);
    if (roll < Const_METypes::CHANCE_BARBARIAN) return UnitType::Barbarian;
    if (roll < Const_METypes::CHANCE_WARRIOR) return UnitType::Warrior;
    return UnitType::Wizard;
}

void HexMap::spawnEnemyInHex(Hex& hex, bool isDangerZone)
{
    QPoint currPos(hex.q, hex.r);
    int level = calculateUnitLevel(hex.q, hex.r, isDangerZone);

    UnitType type = chooseRandomEnemyType();

    Unit* newUnit = unitFabric.create(type, level, currPos);
    if (newUnit)
    {
        hex.setUnit(newUnit);
        if (newUnit->isEnemy())
        {
            enemyCounter++;
        }
    }
}

void HexMap::generateUnits()
{
    PerlinNoise terrainNoise(mapSeed);
    PerlinNoise dangerNoise(mapSeed + Noise::DANGER_NOISE_OFFSET);

    placeGuaranteedCampfire();

    std::vector<Hex*> emptyHexes;

    for(auto& Col : mapGrid)
    {
        for(Hex& Hex_ : Col)
        {
            if(Hex_.q == 0 && Hex_.r == 0) continue;
            if(Hex_.haveUnit()) continue;

            double terrainVal = terrainNoise.octaveNoise(Hex_.q * Noise::SCALE_TERRAIN,
                                                         Hex_.r * Noise::SCALE_TERRAIN,
                                                         Noise::TERRAIN_NOISE_OCTAVES_NUM,
                                                         Noise::TERRAIN_NOISE_PERSISTENCE);

            if (terrainVal > Noise::THRESHOLD_MOUNTAIN) {
                Unit* mtn = unitFabric.create(UnitType::StructUnBreak, 1, QPoint(Hex_.q, Hex_.r));
                if(mtn) Hex_.setUnit(mtn);
                continue;
            }

            double dangerVal = dangerNoise.noise(Hex_.q * Noise::SCALE_DANGER,
                                                 Hex_.r * Noise::SCALE_DANGER);

            bool isDangerZone = dangerVal > Noise::THRESHOLD_DANGER_ZONE;

            double currentEnemyChance = isDangerZone ? Noise::CHANCE_ENEMY_IN_ZONE
                                                     : Noise::CHANCE_ENEMY_ROAMING;

            if (RandGenerator::randDoubleInInterval(0.0, 1.0) < currentEnemyChance) {
                spawnEnemyInHex(Hex_, isDangerZone);
                continue;
            }

            emptyHexes.push_back(&Hex_);
        }
    }

    std::shuffle(emptyHexes.begin(), emptyHexes.end(), std::mt19937(mapSeed));

    int totalEmpty = emptyHexes.size();
    int friendsCount = std::max(1, static_cast<int>(totalEmpty * GarantUnit::CHANCE_FRIEND));
    int chestsCount  = std::max(1, static_cast<int>(totalEmpty * GarantUnit::CHANCE_CHEST));
    int campfiresCount = std::max(1, static_cast<int>(totalEmpty * GarantUnit::CHANCE_CAMPFIRE));

    int currentIndex = 0;

    for (int i = 0; i < friendsCount && currentIndex < totalEmpty; ++i) {
        Hex* h = emptyHexes[currentIndex++];
        Unit* u = unitFabric.create(UnitType::Friend, 1, QPoint(h->q, h->r));
        if (u) h->setUnit(u);
    }

    for (int i = 0; i < chestsCount && currentIndex < totalEmpty; ++i) {
        Hex* h = emptyHexes[currentIndex++];
        Unit* u = unitFabric.create(UnitType::StructBreak, 1, QPoint(h->q, h->r));
        if (u) h->setUnit(u);
    }

    std::vector<QPoint> existingCampfires;

    for (int i = 0; i < campfiresCount && currentIndex < totalEmpty; ++i) {
        Hex* h = emptyHexes[currentIndex++];

        bool tooClose = false;
        for (const auto& pos : existingCampfires) {
            if (getHexDistance(h->q - pos.x(), h->r - pos.y()) < 4) {
                tooClose = true;
                break;
            }
        }

        if (!tooClose) {
            Unit* u = unitFabric.create(UnitType::CampfireUnit, 1, QPoint(h->q, h->r));
            if (u) {
                h->setUnit(u);
                existingCampfires.push_back(QPoint(h->q, h->r));
            }
        }
    }
}

Hex& HexMap::getChangeableLocation(int q, int r)
{
    int qi = q + radiusInner;
    if(qi < 0 || qi >= mapGrid.size())
    {
        throw std::out_of_range("Coordinate q out of range");
    }

    auto& Col = mapGrid[qi];
    int r1 = std::max(-radiusInner, -q - radiusInner);

    int index = r - r1;

    if(index < 0 || index >= Col.size()) {
        throw std::out_of_range("Coordinate r out of range");
    }

    if (Col[index].q != q || Col[index].r != r) {
        throw std::logic_error("Map indexing logic error");
    }

    return Col[index];
}

Hex& HexMap::getChangeableQPointLoc(const QPoint& OHex)
{
    return getChangeableLocation(OHex.x(), OHex.y());
}

const Hex& HexMap::getLocation(int q, int r) const
{
    int qi = q + radiusInner;
    if(qi < 0 || qi >= mapGrid.size())
    {
        throw std::out_of_range("Coordinate q out of range");
    }

    const auto& Col = mapGrid[qi];
    int r1 = std::max(-radiusInner, -q - radiusInner);

    int index = r - r1;

    if(index < 0 || index >= Col.size()) {
        throw std::out_of_range("Coordinate r out of range");
    }

    if (Col[index].q != q || Col[index].r != r) {
        throw std::logic_error("Map indexing logic error");
    }

    return Col[index];
}

const Hex& HexMap::getQPointLoc(const QPoint& OHex) const
{
    return getLocation(OHex.x(), OHex.y());
}

bool HexMap::containsHex(int q, int r) const
{
    int qi = q + radiusInner;
    if(qi < 0 || qi >= mapGrid.size())
        return false;

    int r1 = std::max(-radiusInner, -q - radiusInner);
    int r2 = std::min(radiusInner, -q + radiusInner);

    return (r >= r1 && r <= r2);
}

void HexMap::updateVisibility(const QPoint& HeroPos)
{
    if(!visibleNow.empty())
    {
        for(auto& Hex_ : visibleNow)
        {
            Hex_->isVisible = false;
        }
    }
    visibleNow.clear();
    visibleNow.reserve(7);

    Hex& CenterHex = getChangeableQPointLoc(HeroPos);
    CenterHex.isVisible = true;
    CenterHex.isExplored = true;
    visibleNow.emplace_back(&CenterHex);

    static const std::vector<QPoint> NeighborOffsets = {
        {1, 0}, {1, -1}, {0, -1}, {-1, 0}, {-1, 1}, {0, 1}
    };

    for (const auto& offset : NeighborOffsets)
    {
        int nq = HeroPos.x() + offset.x();
        int nr = HeroPos.y() + offset.y();

        if (containsHex(nq, nr))
        {
            Hex& neighbor = getChangeableLocation(nq, nr);
            neighbor.isVisible = true;
            neighbor.isExplored = true;
            visibleNow.emplace_back(&neighbor);
        }
    }
}

void HexMap::saveToFile(const QString& filePath, const MainHero& hero) const
{
    QJsonObject root;

    root["radius"] = radiusInner;
    root["enemyCount"] = enemyCounter;
    root["mapSeed"] = static_cast<qint64>(mapSeed);

    root["hero"] = hero.toJson();

    QJsonArray hexArray;
    for (const auto& Col : mapGrid) {
        for (const Hex& Hex_ : Col) {
            QJsonObject hexObj;
            hexObj["q"] = Hex_.q;
            hexObj["r"] = Hex_.r;
            hexObj["vis"] = Hex_.isVisible;
            hexObj["exp"] = Hex_.isExplored;

            if (Hex_.haveUnit()) {
                hexObj["unit"] = Hex_.getUnit()->toJson();
            }
            hexArray.append(hexObj);
        }
    }
    root["map"] = hexArray;

    QFile file(filePath);
    if (file.open(QIODevice::WriteOnly)) {
        QJsonDocument doc(root);
        file.write(doc.toJson());
        file.close();
        qDebug() << "Game saved to JSON:" << filePath;
    } else {
        qDebug() << "Failed to save game:" << filePath;
    }
}
bool HexMap::loadFromFile(const QString& filePath, MainHero& hero)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        qDebug() << "Failed to open save file:" << filePath;
        return false;
    }

    QByteArray data = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(data);
    if (doc.isNull()) return false;

    QJsonObject root = doc.object();

    clear();

    this->radiusInner = root["radius"].toInt();
    this->enemyCounter = root["enemyCount"].toInt();
    if (root.contains("mapSeed")) {
        this->mapSeed = static_cast<unsigned int>(root["mapSeed"].toVariant().toLongLong());
    }

    for(int q = -radiusInner; q <= radiusInner; q++) {
        int r1 = std::max(-radiusInner, -q - radiusInner);
        int r2 = std::min(radiusInner, -q + radiusInner);
        std::vector<Hex> Column;
        for(int r = r1; r <= r2; r++) {
            Column.emplace_back(q, r);
        }
        mapGrid.push_back(std::move(Column));
    }

    if (root.contains("hero")) {
        hero.fromJson(root["hero"].toObject());
    }

    QJsonArray hexArray = root["map"].toArray();
    for (const auto& val : hexArray) {
        QJsonObject hexObj = val.toObject();
        int q = hexObj["q"].toInt();
        int r = hexObj["r"].toInt();

        if (!containsHex(q, r)) continue;

        Hex& hex = getChangeableLocation(q, r);
        hex.isVisible = hexObj["vis"].toBool();
        hex.isExplored = hexObj["exp"].toBool();

        if (hexObj.contains("unit")) {
            QJsonObject unitObj = hexObj["unit"].toObject();

            int typeInt = unitObj["type"].toInt();
            UnitType type = static_cast<UnitType>(typeInt);
            double level = unitObj["level"].toDouble();

            Unit* newUnit = unitFabric.create(type, level, QPoint(q, r));

            if (newUnit) {
                newUnit->fromJson(unitObj);
                hex.setUnit(newUnit);
            }
        }
    }

    updateVisibility(hero.getPosition());
    qDebug() << "Game loaded from JSON successfully.";
    return true;
}

void HexMap::clear()
{
    visibleNow.clear();
    for(auto& Col : mapGrid)
    {
        for(Hex& Hex_ : Col)
        {
            Hex_.clearUnit();
        }
    }
    mapGrid.clear();
    unitFabric.clearAll();
    enemyCounter = 0;
}

void HexMap:: clearUnitAt(const QPoint& position)
{
    if (containsHex(position.x(), position.y()))
    {
        Hex& h = getChangeableQPointLoc(position);
        if (h.haveUnit())
        {
            Unit* u = h.getUnit();
            if (u->isEnemy())
            {
                decrementEnemyCount();
            }
            h.clearUnit();
        }
    }
    else
    {
        qDebug() << "HexMap::ClearUnitAt: Attempted to clear unit at invalid position" << position;
    }
}

int HexMap::getHexDistance(int q, int r) const
{
    return (std::abs(q)+std::abs(r)+std::abs(q+r)) / 2;
}


int HexMap::getEnemyCount() const
{
    return enemyCounter;
}

void HexMap::decrementEnemyCount()
{
    if (enemyCounter > 0)
    {
        enemyCounter--;
    }
    qDebug() << "enemyCounter decremented. Current count: " << enemyCounter;
}

struct PathNode {
    QPoint pos;
    int g;
    int h;
    PathNode* parent;

    PathNode(QPoint p, int _g, int _h, PathNode* _parent = nullptr)
        : pos(p), g(_g), h(_h), parent(_parent) {}

    int f() const { return g + h; }
};

struct CompareNode {
    bool operator()(const PathNode* a, const PathNode* b) {
        return a->f() > b->f();
    }
};

std::vector<QPoint> HexMap::findPath(QPoint start, QPoint target)
{
    if (!containsHex(target.x(), target.y())) return {};
    if (start == target) return {};

    const Hex& targetHex = getQPointLoc(target);

    if (!targetHex.isExplored) return {};

    if (targetHex.haveUnit()) {
        Unit* u = targetHex.getUnit();
        if (u->isStructure() || u->isEnemy()) {
            return {};
        }
    }

    std::priority_queue<PathNode*, std::vector<PathNode*>, CompareNode> openList;

    QHash<QPoint, PathNode*> allNodes;
    std::vector<QPoint> path;

    PathNode* startNode = new PathNode(start, 0, getHexDistance(start.x() - target.x(), start.y() - target.y()));
    openList.push(startNode);
    allNodes.insert(start, startNode);

    PathNode* finalNode = nullptr;

    static const std::vector<QPoint> NeighborOffsets = {
        {1, 0}, {1, -1}, {0, -1}, {-1, 0}, {-1, 1}, {0, 1}
    };

    while (!openList.empty()) {
        PathNode* current = openList.top();
        openList.pop();

        if (current->pos == target) {
            finalNode = current;
            break;
        }

        for (const auto& offset : NeighborOffsets) {
            QPoint nextPos = current->pos + offset;

            if (!containsHex(nextPos.x(), nextPos.y())) continue;

            const Hex& neighborHex = getQPointLoc(nextPos);

            if (!neighborHex.isExplored) continue;

            bool isWalkable = true;
            if (neighborHex.haveUnit()) {
                Unit* u = neighborHex.getUnit();
                if (u->isStructure() || u->isEnemy()) {
                    isWalkable = false;
                }
            }

            if (!isWalkable && nextPos != target) continue;
            if (!isWalkable && nextPos == target) continue;

            int newG = current->g + 1;

            if (!allNodes.contains(nextPos)) {
                int newH = getHexDistance(nextPos.x() - target.x(), nextPos.y() - target.y());
                PathNode* neighbor = new PathNode(nextPos, newG, newH, current);
                allNodes.insert(nextPos, neighbor);
                openList.push(neighbor);
            } else {
                PathNode* existing = allNodes.value(nextPos);
                if (newG < existing->g) {
                    existing->g = newG;
                    existing->parent = current;
                    openList.push(existing);
                }
            }
        }
    }

    if (finalNode) {
        PathNode* curr = finalNode;
        while (curr != nullptr) {
            path.push_back(curr->pos);
            curr = curr->parent;
        }
        std::reverse(path.begin(), path.end());

        if (!path.empty() && path.front() == start) {
            path.erase(path.begin());
        }
    }

    qDeleteAll(allNodes);

    return path;
}
