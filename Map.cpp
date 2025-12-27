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

int HexMap::GetRadius() const { return Radius; }
const std::vector<std::vector<Hex>>& HexMap::GetMap() const { return MapGrid; }


HexMap::HexMap(int radius) : Radius(radius), EnemyCounter(0)
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
        MapGrid.push_back(Column);
    }
    GenerateUnits();

    QPoint S = {0, 0};
    UpdateVisibility(S);
}

void HexMap::PlaceGuaranteedCampfire()
{
    std::vector<QPoint> Neighbors = {{1,0}, {1,-1}, {0,-1}, {-1,0}, {-1,1}, {0,1}};
    std::vector<QPoint> ValidPositions;

    for(const auto& offset : Neighbors)
    {
        if(ContainsHex(offset.x(), offset.y()))
        {
            ValidPositions.push_back(offset);
        }
    }

    if(!ValidPositions.empty())
    {
        int idx = RandGenerator::RandIntInInterval(0, ValidPositions.size() - 1);
        QPoint pos = ValidPositions[idx];

        Hex& hex = GetChangeableQPointLoc(pos);
        Unit* campfire = UnitFabric_.Create(UnitType::CampfireUnit, 1, pos);
        if(campfire)
        {
            hex.SetUnit(campfire);
        }
    }
}

int HexMap::CalculateUnitLevel(int q, int r, bool isDangerZone) const
{
    int dist = GetHexDistance(q, r);
    double distRatio = static_cast<double>(dist) / Radius;

    double baseLevel = Const_MZones::DISTANCE_BASE + (distRatio * Const_MZones::DISTANCE_MULT);

    int variance = RandGenerator::RandIntInInterval(Const_MZones::RAND_NEG_BONUS, Const_MZones::RAND_POS_BONUS);

    int dangerBonus = isDangerZone ? Const_MZones::DANGER_BONUS : Const_MZones::NO_DANGER_BONUS;

    int finalLevel = static_cast<int>(baseLevel) + variance + dangerBonus;

    if (finalLevel < 1) finalLevel = 1;

    return finalLevel;
}

UnitType HexMap::ChooseRandomEnemyType() const
{
    double roll = RandGenerator::RandDoubleInInterval(0.0, 1.0);
    if (roll < Const_METypes::CHANCE_BARBARIAN) return UnitType::Barbarian;
    if (roll < Const_METypes::CHANCE_WARRIOR) return UnitType::Warrior;
    return UnitType::Wizard;
}

void HexMap::SpawnEnemyInHex(Hex& hex, bool isDangerZone)
{
    QPoint currPos(hex.q, hex.r);
    int level = CalculateUnitLevel(hex.q, hex.r, isDangerZone);

    UnitType type = ChooseRandomEnemyType();

    Unit* newUnit = UnitFabric_.Create(type, level, currPos);
    if (newUnit)
    {
        hex.SetUnit(newUnit);
        if (newUnit->IsEnemy())
        {
            EnemyCounter++;
        }
    }
}

void HexMap::GenerateUnits()
{
    PerlinNoise terrainNoise(mapSeed);
    PerlinNoise dangerNoise(mapSeed + Noise::DANGER_NOISE_OFFSET);

    PlaceGuaranteedCampfire();

    std::vector<Hex*> emptyHexes;

    for(auto& Col : MapGrid)
    {
        for(Hex& Hex_ : Col)
        {
            if(Hex_.q == 0 && Hex_.r == 0) continue;
            if(Hex_.HaveUnit()) continue;

            double terrainVal = terrainNoise.octaveNoise(Hex_.q * Noise::SCALE_TERRAIN,
                                                         Hex_.r * Noise::SCALE_TERRAIN,
                                                         Noise::TERRAIN_NOISE_OCTAVES_NUM,
                                                         Noise::TERRAIN_NOISE_PERSISTENCE);

            if (terrainVal > Noise::THRESHOLD_MOUNTAIN) {
                Unit* mtn = UnitFabric_.Create(UnitType::StructUnBreak, 1, QPoint(Hex_.q, Hex_.r));
                if(mtn) Hex_.SetUnit(mtn);
                continue;
            }

            double dangerVal = dangerNoise.noise(Hex_.q * Noise::SCALE_DANGER,
                                                 Hex_.r * Noise::SCALE_DANGER);

            bool isDangerZone = dangerVal > Noise::THRESHOLD_DANGER_ZONE;

            double currentEnemyChance = isDangerZone ? Noise::CHANCE_ENEMY_IN_ZONE
                                                     : Noise::CHANCE_ENEMY_ROAMING;

            if (RandGenerator::RandDoubleInInterval(0.0, 1.0) < currentEnemyChance) {
                SpawnEnemyInHex(Hex_, isDangerZone);
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
        Unit* u = UnitFabric_.Create(UnitType::Friend, 1, QPoint(h->q, h->r));
        if (u) h->SetUnit(u);
    }

    for (int i = 0; i < chestsCount && currentIndex < totalEmpty; ++i) {
        Hex* h = emptyHexes[currentIndex++];
        Unit* u = UnitFabric_.Create(UnitType::StructBreak, 1, QPoint(h->q, h->r));
        if (u) h->SetUnit(u);
    }

    std::vector<QPoint> existingCampfires;

    for (int i = 0; i < campfiresCount && currentIndex < totalEmpty; ++i) {
        Hex* h = emptyHexes[currentIndex++];

        bool tooClose = false;
        for (const auto& pos : existingCampfires) {
            if (GetHexDistance(h->q - pos.x(), h->r - pos.y()) < 4) {
                tooClose = true;
                break;
            }
        }

        if (!tooClose) {
            Unit* u = UnitFabric_.Create(UnitType::CampfireUnit, 1, QPoint(h->q, h->r));
            if (u) {
                h->SetUnit(u);
                existingCampfires.push_back(QPoint(h->q, h->r));
            }
        }
    }
}

Hex& HexMap::GetChangeableLocation(int q, int r)
{
    int qi = q + Radius;
    if(qi < 0 || qi >= MapGrid.size())
    {
        throw std::out_of_range("Coordinate q out of range");
    }

    auto& Col = MapGrid[qi];
    int r1 = std::max(-Radius, -q - Radius);

    int index = r - r1;

    if(index < 0 || index >= Col.size()) {
        throw std::out_of_range("Coordinate r out of range");
    }

    if (Col[index].q != q || Col[index].r != r) {
        throw std::logic_error("Map indexing logic error");
    }

    return Col[index];
}

Hex& HexMap::GetChangeableQPointLoc(const QPoint& OHex)
{
    return GetChangeableLocation(OHex.x(), OHex.y());
}

const Hex& HexMap::GetLocation(int q, int r) const
{
    int qi = q + Radius;
    if(qi < 0 || qi >= MapGrid.size())
    {
        throw std::out_of_range("Coordinate q out of range");
    }

    const auto& Col = MapGrid[qi];
    int r1 = std::max(-Radius, -q - Radius);

    int index = r - r1;

    if(index < 0 || index >= Col.size()) {
        throw std::out_of_range("Coordinate r out of range");
    }

    if (Col[index].q != q || Col[index].r != r) {
        throw std::logic_error("Map indexing logic error");
    }

    return Col[index];
}

const Hex& HexMap::GetQPointLoc(const QPoint& OHex) const
{
    return GetLocation(OHex.x(), OHex.y());
}

bool HexMap::ContainsHex(int q, int r) const
{
    int qi = q + Radius;
    if(qi < 0 || qi >= MapGrid.size())
        return false;

    int r1 = std::max(-Radius, -q - Radius);
    int r2 = std::min(Radius, -q + Radius);

    return (r >= r1 && r <= r2);
}

void HexMap::UpdateVisibility(const QPoint& HeroPos)
{
    if(!visibleNow.empty())
    {
        for(auto& Hex_ : visibleNow)
        {
            Hex_->IsVisible = false;
        }
    }
    visibleNow.clear();
    visibleNow.reserve(7);

    Hex& CenterHex = GetChangeableQPointLoc(HeroPos);
    CenterHex.IsVisible = true;
    CenterHex.IsExplored = true;
    visibleNow.emplace_back(&CenterHex);

    static const std::vector<QPoint> NeighborOffsets = {
        {1, 0}, {1, -1}, {0, -1}, {-1, 0}, {-1, 1}, {0, 1}
    };

    for (const auto& offset : NeighborOffsets)
    {
        int nq = HeroPos.x() + offset.x();
        int nr = HeroPos.y() + offset.y();

        if (ContainsHex(nq, nr))
        {
            Hex& neighbor = GetChangeableLocation(nq, nr);
            neighbor.IsVisible = true;
            neighbor.IsExplored = true;
            visibleNow.emplace_back(&neighbor);
        }
    }
}

void HexMap::SaveToFile(const QString& filePath, const MainHero& hero) const
{
    QJsonObject root;

    root["radius"] = Radius;
    root["enemyCount"] = EnemyCounter;
    root["mapSeed"] = static_cast<qint64>(mapSeed);

    root["hero"] = hero.ToJson();

    QJsonArray hexArray;
    for (const auto& Col : MapGrid) {
        for (const Hex& Hex_ : Col) {
            QJsonObject hexObj;
            hexObj["q"] = Hex_.q;
            hexObj["r"] = Hex_.r;
            hexObj["vis"] = Hex_.IsVisible;
            hexObj["exp"] = Hex_.IsExplored;

            if (Hex_.HaveUnit()) {
                hexObj["unit"] = Hex_.GetUnit()->ToJson();
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
bool HexMap::LoadFromFile(const QString& filePath, MainHero& hero)
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

    Clear();

    this->Radius = root["radius"].toInt();
    this->EnemyCounter = root["enemyCount"].toInt();
    if (root.contains("mapSeed")) {
        this->mapSeed = static_cast<unsigned int>(root["mapSeed"].toVariant().toLongLong());
    }

    for(int q = -Radius; q <= Radius; q++) {
        int r1 = std::max(-Radius, -q - Radius);
        int r2 = std::min(Radius, -q + Radius);
        std::vector<Hex> Column;
        for(int r = r1; r <= r2; r++) {
            Column.emplace_back(q, r);
        }
        MapGrid.push_back(std::move(Column));
    }

    if (root.contains("hero")) {
        hero.FromJson(root["hero"].toObject());
    }

    QJsonArray hexArray = root["map"].toArray();
    for (const auto& val : hexArray) {
        QJsonObject hexObj = val.toObject();
        int q = hexObj["q"].toInt();
        int r = hexObj["r"].toInt();

        if (!ContainsHex(q, r)) continue;

        Hex& hex = GetChangeableLocation(q, r);
        hex.IsVisible = hexObj["vis"].toBool();
        hex.IsExplored = hexObj["exp"].toBool();

        if (hexObj.contains("unit")) {
            QJsonObject unitObj = hexObj["unit"].toObject();

            int typeInt = unitObj["type"].toInt();
            UnitType type = static_cast<UnitType>(typeInt);
            double level = unitObj["level"].toDouble();

            Unit* newUnit = UnitFabric_.Create(type, level, QPoint(q, r));

            if (newUnit) {
                newUnit->FromJson(unitObj);
                hex.SetUnit(newUnit);
            }
        }
    }

    UpdateVisibility(hero.GetPosition());
    qDebug() << "Game loaded from JSON successfully.";
    return true;
}

void HexMap::Clear()
{
    visibleNow.clear();
    for(auto& Col : MapGrid)
    {
        for(Hex& Hex_ : Col)
        {
            Hex_.ClearUnit();
        }
    }
    MapGrid.clear();
    UnitFabric_.ClearAll();
    EnemyCounter = 0;
}

void HexMap:: ClearUnitAt(const QPoint& position)
{
    if (ContainsHex(position.x(), position.y()))
    {
        Hex& h = GetChangeableQPointLoc(position);
        if (h.HaveUnit())
        {
            Unit* u = h.GetUnit();
            if (u->IsEnemy())
            {
                DecrementEnemyCount();
            }
            h.ClearUnit();
        }
    }
    else
    {
        qDebug() << "HexMap::ClearUnitAt: Attempted to clear unit at invalid position" << position;
    }
}

int HexMap::GetHexDistance(int q, int r) const
{
    return (std::abs(q)+std::abs(r)+std::abs(q+r)) / 2;
}


int HexMap::GetEnemyCount() const
{
    return EnemyCounter;
}

void HexMap::DecrementEnemyCount()
{
    if (EnemyCounter > 0)
    {
        EnemyCounter--;
    }
    qDebug() << "EnemyCounter decremented. Current count: " << EnemyCounter;
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

std::vector<QPoint> HexMap::FindPath(QPoint start, QPoint target)
{
    if (!ContainsHex(target.x(), target.y())) return {};
    if (start == target) return {};

    const Hex& targetHex = GetQPointLoc(target);

    if (!targetHex.IsExplored) return {};

    if (targetHex.HaveUnit()) {
        Unit* u = targetHex.GetUnit();
        if (u->IsStructure() || u->IsEnemy()) {
            return {};
        }
    }

    std::priority_queue<PathNode*, std::vector<PathNode*>, CompareNode> openList;

    QHash<QPoint, PathNode*> allNodes;
    std::vector<QPoint> path;

    PathNode* startNode = new PathNode(start, 0, GetHexDistance(start.x() - target.x(), start.y() - target.y()));
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

            if (!ContainsHex(nextPos.x(), nextPos.y())) continue;

            const Hex& neighborHex = GetQPointLoc(nextPos);

            if (!neighborHex.IsExplored) continue;

            bool isWalkable = true;
            if (neighborHex.HaveUnit()) {
                Unit* u = neighborHex.GetUnit();
                if (u->IsStructure() || u->IsEnemy()) {
                    isWalkable = false;
                }
            }

            if (!isWalkable && nextPos != target) continue;
            if (!isWalkable && nextPos == target) continue;

            int newG = current->g + 1;

            if (!allNodes.contains(nextPos)) {
                int newH = GetHexDistance(nextPos.x() - target.x(), nextPos.y() - target.y());
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
