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

    QPixmap HeroPixmap;
    QPixmap StandartVisibleHexTexture;
    QPixmap StandartExploredHexTexture;
    QPixmap FogTexture;
    QPixmap BarbarianTexture;
    QPixmap WarriorTexture;
    QPixmap WizardTexture;
    QPixmap FriendTexture;
    QPixmap StructBreakTexture;
    QPixmap StructUnBreakTexture;
    QPixmap CampfireTexture;
    QPixmap HeroWithWarriorTexture;
    QPixmap HeroWithWizardTexture;
    QPixmap HeroWithBarbarianTexture;
    QPixmap HeroWithFriendTexture;
    QPixmap HeroWithStructTexture;
    QPixmap HeroWithCampfireTexture;

    void InitializeTextures();
    QPixmap loadTexture(const QString &fileName, double scaleFactor);
    QPixmap TintPixmap(const QPixmap& Source, qreal Strength = 0.4);
    QPixmap getEnemyTexture(UnitType type);
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

    QPixmap getTerrainTexture(bool visible, bool explored);
    QPixmap getUnitTexture(UnitType type, bool isHeroOnHex);

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

signals:
    void heroStatsChanged();
    void gameOver();
    void victory();
    void logMessage(const QString& msg);
};

#endif // GAMESCENE_H
