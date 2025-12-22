#ifndef GAMESCENE_H
#define GAMESCENE_H

#include <QGraphicsScene>
#include <QMap>
#include <QTimer>
#include <vector>
#include <QPoint>
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
    QPoint pendingTarget = QPoint(-999, -999);
    std::vector<HexItem*> highlightedPath;
    QTimer* movementTimer;
    std::vector<QPoint> currentPath;
    bool isMoving;

    QWidget* getViewWidget();

    void generateMapItems();
    void refreshMap();

    bool tryMoveHeroTo(const QPoint& targetHexCoords);
    void interactWithContentOnHex(const Hex& hex, const QPoint& previousPos);
    void processCombat(Unit* enemy, const QPoint& previousPos);
    void processCampfire(Unit* campfireUnit);
    void processFriendly(Unit* friendUnit);
    void processTreasure(Unit* treasureUnit);

    void clearPathHighlight();
    void highlightPath();

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

    void StartMovement();
    bool IsHeroMoving() const { return isMoving; }

signals:
    void heroStatsChanged();
    void gameOver();
    void victory();
    void logMessage(const QString& msg);

public slots:
    void processStep();
};

#endif // GAMESCENE_H
