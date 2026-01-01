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
    Hex* hexInner;
    GameScene* sceneInner;
    bool isPathHighlighted = false;

    void createPolygon();
    void drawLevelBadge(QPainter* painter, int level);


protected:
    void paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget) override;
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void hoverEnterEvent(QGraphicsSceneHoverEvent* event) override;
    void hoverLeaveEvent(QGraphicsSceneHoverEvent* event) override;

public:
    HexItem(Hex* modelHex, GameScene* scene, QGraphicsItem* parent = nullptr);
    Hex* getModelHex() const { return hexInner; }
    void updateState();
    void updateZValue();
    void setPathHighlight(bool active);
};

#endif // HEXITEM_H
