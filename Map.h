#ifndef MAP_H_DEFINED
#define MAP_H_DEFINED

#include "Cell.h"
#include "UnitFabric.h"
#include <iostream>
#include <vector>
#include <QJsonObject>
#include <QJsonArray>
#include <QJsonDocument>
#include <QFile>

class HexMap
{
private:
    int radiusInner;
    std::vector<std::vector<Hex>> mapGrid;
    UnitFabric unitFabric;
    int enemyCounter=0;
    std::vector<Hex*> visibleNow;
    unsigned int mapSeed;

    int getHexDistance(int q, int r) const;
    Hex& getChangeableLocation(int q, int r);
    Hex& getChangeableQPointLoc(const QPoint& OHex);

    void generateUnits();
    void placeGuaranteedCampfire();
    void spawnEnemyInHex(Hex& hex, bool isDangerZone);
    int calculateUnitLevel(int q, int r, bool isDangerZone) const;
    UnitType chooseRandomEnemyType() const;

public:
    HexMap(int radius);
    int getRadius() const;
    const Hex& getLocation(int q, int r) const;
    const Hex& getQPointLoc(const QPoint& OHex) const;
    const std::vector<std::vector<Hex>>& getMap() const;
    bool containsHex(int q, int r) const;
    void updateVisibility(const QPoint& HeroPos);

    void saveToFile(const QString& filePath, const MainHero& hero) const;
    bool loadFromFile(const QString& filePath, MainHero& hero);

    void clear();
    void clearUnitAt(const QPoint& position);

    int getEnemyCount() const;
    void decrementEnemyCount();

    unsigned int getSeed() const { return mapSeed; }

    std::vector<QPoint> findPath(QPoint start, QPoint target); //A*
};

#endif //MAP_H_DEFINED
