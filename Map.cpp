#include "Map.h"
#include <QFile>
#include <QDataStream>
#include <QIODevice>
#include <QString>
#include <QDebug>
#include "RandomGenerator.h"
#include "GameConstants.h"

namespace Const_MGen = GlobalConst::MapGeneration;
namespace Const_MZones = GlobalConst::MapGeneration::Zones;
namespace Const_MProbs = GlobalConst::MapGeneration::UnitProbabilities;
namespace Const_METypes = GlobalConst::MapGeneration::EnemyTypes;

int HexMap::GetRadius() const { return Radius; }
const std::vector<std::vector<Hex>>& HexMap::GetMap() const { return MapGrid; }


HexMap::HexMap(int radius) : Radius(radius), EnemyCounter(0)
{
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

int HexMap::CalculateZoneLevel(int distance) const
{
    // Розбиваємо карту на зони складності
    int Zone1 = static_cast<int>(Radius * Const_MZones::ZONE_1_RATIO);
    int Zone2 = static_cast<int>(Radius * Const_MZones::ZONE_2_RATIO);

    if (distance <= Zone1) return RandGenerator::RandIntInInterval(1, 2);
    if (distance <= Zone2) return RandGenerator::RandIntInInterval(3, 4);
    return RandGenerator::RandIntInInterval(5, 6);
}

UnitType HexMap::ChooseRandomEnemyType() const
{
    // Шанси появи конкретних ворогів
    double roll = RandGenerator::RandDoubleInInterval(0.0, 1.0);
    if (roll < Const_METypes::CHANCE_BARBARIAN) return UnitType::Barbarian;
    if (roll < Const_METypes::CHANCE_WARRIOR) return UnitType::Warrior;
    return UnitType::Wizard;
}

UnitType HexMap::ChooseRandomUnitType() const
{
    // 60% Enemy, 25% Unbreak, 7% Break, 7% Friend, 1% Campfire
    double roll = RandGenerator::RandDoubleInInterval(0.0, 1.0);

    if (roll < Const_MProbs::THRESHOLD_ENEMY) return UnitType::Enemy;
    if (roll < Const_MProbs::THRESHOLD_UNBREAK) return UnitType::StructUnBreak;
    if (roll < Const_MProbs::THRESHOLD_BREAK) return UnitType::StructBreak;
    if (roll < Const_MProbs::THRESHOLD_FRIEND) return UnitType::Friend;
    return UnitType::CampfireUnit;
}

void HexMap::SpawnUnitInHex(Hex& hex, const QPoint& protectedPos)
{
    const double SpawnChance = Const_MGen::SPAWN_CHANCE;
    if (RandGenerator::RandDoubleInInterval(0.0, 1.0) > SpawnChance) return;

    QPoint currPos(hex.q, hex.r);

    int distance = GetHexDistance(currPos.x(), currPos.y());
    int level = CalculateZoneLevel(distance);

    UnitType type = ChooseRandomUnitType();

    if (type == UnitType::Enemy)
    {
        type = ChooseRandomEnemyType();
    }

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
    PlaceGuaranteedCampfire();

    QPoint HeroSpawn(0, 0);

    for(auto& Col : MapGrid)
    {
        for(Hex& Hex_ : Col)
        {
            if(Hex_.q == 0 && Hex_.r == 0) continue;
            if(Hex_.HaveUnit()) continue;
            SpawnUnitInHex(Hex_, HeroSpawn);
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
    for(auto& Col : MapGrid)
    {
        for(auto& Hex_ : Col)
        {
            Hex_.IsVisible = false;
        }
    }

    Hex& CenterHex = GetChangeableQPointLoc(HeroPos);
    CenterHex.IsVisible = true;
    CenterHex.IsExplored = true;

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
        }
    }
}

void HexMap::SaveToFile(const QString& filePath, const QPoint& heroPos, double HeroHP,double HeroMP, double HeroLVL) const
{
    QJsonObject root;

    root["radius"] = Radius;
    root["enemyCount"] = EnemyCounter;

    QJsonObject heroObj;
    heroObj["x"] = heroPos.x();
    heroObj["y"] = heroPos.y();
    heroObj["hp"] = HeroHP;
    heroObj["mana"] = HeroMP;
    heroObj["level"] = HeroLVL;
    root["hero"] = heroObj;

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
bool HexMap::LoadFromFile(const QString& filePath, QPoint& heroPos, double& HeroHP,double& HeroMP, double& HeroLVL)
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

    for(int q = -Radius; q <= Radius; q++) {
        int r1 = std::max(-Radius, -q - Radius);
        int r2 = std::min(Radius, -q + Radius);
        std::vector<Hex> Column;
        for(int r = r1; r <= r2; r++) {
            Column.emplace_back(q, r);
        }
        MapGrid.push_back(std::move(Column));
    }

    QJsonObject heroObj = root["hero"].toObject();
    heroPos = QPoint(heroObj["x"].toInt(), heroObj["y"].toInt());
    HeroHP = heroObj["hp"].toDouble();
    HeroMP = heroObj["mana"].toDouble();
    HeroLVL = heroObj["level"].toDouble();

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

    UpdateVisibility(heroPos);
    qDebug() << "Game loaded from JSON successfully.";
    return true;
}

void HexMap::Clear()
{
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
