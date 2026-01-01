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
    HexMap mapInner;
    MainHero heroInner;
    QHash<QPoint, HexItem*> hexItemsMap;
    bool isPanning = false;
    bool isPaused = false;
    bool isMoving;
    int currentPathIndex = 0;
    QPoint pendingTarget = QPoint(-999, -999);
    QPoint lastPreCombatPos;
    std::vector<HexItem*> highlightedPath;
    std::vector<QPoint> currentPath;
    QTimer* movementTimer;

    QWidget* getViewWidget();

    void generateMapItems();
    void refreshMap();

    bool tryMoveHeroTo(const QPoint& targetHexCoords, bool npc_interaction = true);
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

    void saveMapToFile(const QString& filePath);
    bool loadMapFromFile(const QString& filePath);

    struct HeroStats {
        double hp;
        double mp;
        double lvl;
    };
    HeroStats getStats();

    void setPanning(bool panning);
    bool checkPanning() const;

    void startMovement();
    bool isHeroMoving() const { return isMoving; }

    MainHero* getHero() { return &heroInner; }
    void setPaused(bool paused);
    bool checkPaused() const { return isPaused; }

signals:
    void heroStatsChanged();
    void gameOver();
    void victory();
    void logMessage(const QString& msg);
    void levelUpTriggered();
    void combatStarted();
    void combatEnded();
    void combatRequested(Unit* enemy);
    void campfireRequested(double oldHP, double newHP, double oldMana, double newMana, int charges, Unit* campfireUnit);
    void npcInteractionRequested(Unit* npcUnit, const QString& text);

public slots:
    void processStep();
    void finishCombat(bool playerWon, bool playerEscaped, Unit* enemy);
    void finishCampfireInteraction(Unit* campfireUnit);
    void finishNPCInteraction();
};

#endif // GAMESCENE_H
