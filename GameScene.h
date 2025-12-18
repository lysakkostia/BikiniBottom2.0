#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QMap>
#include "Map.h"
#include "Unit.h"
#include "HexItem.h"

class GameScene : public QGraphicsScene
{
    Q_OBJECT
private:
    HexMap Map;
    MainHero Hero;
    bool MisPanning = false;

    QWidget* getViewWidget();

    void generateMapItems();
    void refreshMap();

    bool tryMoveHeroTo(const QPoint& targetHexCoords);
    void interactWithContentOnHex(const Hex& hex, const QPoint& previousPos);
    void processCombat(Unit* enemy, const QPoint& previousPos);
    void processCampfire(Unit* campfireUnit);
    void processFriendly(Unit* friendUnit);
    void processTreasure(Unit* treasureUnit);

public:
    explicit GameScene(int NRadius, QObject *parent = nullptr);

    void handleHexClick(HexItem* item);

    Hex* getHeroHex();

    void SaveMapToFile(const QString& filePath);
    bool LoadMapFromFile(const QString& filePath);

    struct HeroStats {
        double HP;
        double MP;
        double LVL;
    };
    HeroStats GetStats();

    void setPanning(bool panning);
    bool isPanning() const;

signals:
    void heroStatsChanged();
    void gameOver();
    void victory();
    void logMessage(const QString& msg);
};

#endif // GAMESCENE_H
