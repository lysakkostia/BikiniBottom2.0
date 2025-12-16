#ifndef HEXWIDGET_H
#define HEXWIDGET_H

#include <QWidget>
#include <QPainter>
#include <QWheelEvent>
#include <QMouseEvent>
#include <algorithm>
#include "Map.h"
#include "Unit.h"

class HexWidget : public QWidget{
    Q_OBJECT

private:
    HexMap Map;
    MainHero Hero;

    float Scale = 1.0;
    float OffsetX = 0.0;
    float OffsetY = 0.0;
    bool IsDragging = false;
    bool Initialized = false;
    QPoint LastMousePos;
    QPoint SelectedHex = {-1, -1};
    QPoint CenterHex = QPoint(0, 0);
    QPoint HoveredHex = QPoint(INT_MAX, INT_MAX);

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

    QPixmap loadTexture(const QString &fileName, double scaleFactor);
    void drawHexTerrain(QPainter& painter, const Hex& hex, const QPolygonF& polygon);
    void drawUnitAndUI(QPainter& painter, const Hex& hex, const QPolygonF& polygon);
    QPen getHexOutlinePen(const Hex& hex, const Hex& heroHex);
    void drawLevelBadge(QPainter& painter, const QPointF& center, int level);
    void InitializeTextures();

    void handleLeftClick(const QPointF& pos);
    bool tryMoveHeroTo(const QPoint& targetHexCoords);
    void interactWithContentOnHex(const Hex& hex, const QPoint& previousPos);
    void processCombat(Unit* enemy, const QPoint& previousPos);
    void processCampfire(Unit* campfireUnit);
    void processFriendly(Unit* friendUnit);
    void processTreasure(Unit* treasureUnit);

    QPoint PixelToHex(QPointF p) const;
    QPoint CubeToAxial(float qc, float rc) const;
    QRectF GetMapBoundingRect() const;
    QPixmap TintPixmap(const QPixmap& Source, qreal Strength = 0.4);
    QPixmap GetUnitTexture(UnitType type, bool isHeroOnHex);
    QPixmap getEnemyTexture(UnitType type);

public:
    HexWidget(int NRadius, QWidget* parent = nullptr);
    void SaveMapToFile(const QString& filePath);
    bool LoadMapFromFile(const QString& filePath);
    struct HeroStats{
        double HP;
        double MP;
        double LVL;
    };
    HeroStats GetStats();
    signals:
    void gameOver();
    void victory();
    void heroStatsChanged();


protected:
    void paintEvent(QPaintEvent*) override;
    void wheelEvent(QWheelEvent* event) override;
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent*) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void resizeEvent(QResizeEvent* event) override;
    void leaveEvent(QEvent*) override;
};

#endif // HEXWIDGET_H
