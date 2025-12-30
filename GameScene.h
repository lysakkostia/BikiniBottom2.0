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
    bool isPaused = false;

    HexMap Map;
    MainHero Hero;
    QHash<QPoint, HexItem*> hexItemsMap;
    bool MisPanning = false;
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

    MainHero* GetHero() { return &Hero; }
    void SetPaused(bool paused);
    bool IsPaused() const { return isPaused; }

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
    void FinishCombat(bool playerWon, bool playerEscaped, Unit* enemy);
    void FinishCampfireInteraction(Unit* campfireUnit);
    void FinishNPCInteraction();
};

#endif // GAMESCENE_H
