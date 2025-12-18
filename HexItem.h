#ifndef HEXITEM_H
#define HEXITEM_H

#include <QGraphicsPolygonItem>
#include <QPainter>
#include <QGraphicsSceneMouseEvent>
#include "Map.h"

class GameScene;

class HexItem : public QGraphicsPolygonItem
{
private:
    Hex* MHex;
    GameScene* MScene;

    void createPolygon();
    void drawLevelBadge(QPainter* painter, int level);


protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

public:
    HexItem(Hex* modelHex, GameScene* scene, QGraphicsItem* parent = nullptr);
    Hex* getModelHex() const { return MHex; }
    void updateState();
    void updateZValue();
};

#endif // HEXITEM_H
