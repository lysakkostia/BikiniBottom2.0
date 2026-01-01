#include "HexItem.h"
#include "GameScene.h"
#include "TextureManager.h"
#include <QPen>
#include <QStyleOptionGraphicsItem>
#include <QStyle>

HexItem::HexItem(Hex* modelHex, GameScene* scene, QGraphicsItem* parent)
    : QGraphicsPolygonItem(parent), hexInner(modelHex), sceneInner(scene)
{
    createPolygon();
    setAcceptHoverEvents(true);
    QPointF center = hexInner->getCenter();
    setPos(center);
}

void HexItem::createPolygon()
{
    QPolygonF polygon;
    for (const auto& corner : hexInner->getCorners()) {
        polygon << (corner - hexInner->getCenter());
    }
    setPolygon(polygon);
}

void HexItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPixmap bgTexture = TextureManager::getInstance().getTerrainTexture(hexInner->visibilityState(), hexInner->explorationState());

    painter->save();
    painter->setClipPath(shape());

    if (!bgTexture.isNull()) {
        painter->drawPixmap(-bgTexture.width()/2, -bgTexture.height()/2, bgTexture);
    } else {
        QColor color = Qt::black;
        if (hexInner->visibilityState()) color = Qt::white;
        else if (hexInner->explorationState()) color = Qt::darkGray;
        painter->setBrush(color);
        painter->drawPolygon(polygon());
    }
    painter->restore();

    if (hexInner->visibilityState() || hexInner->explorationState())
    {
        bool isHeroOnHex = (hexInner == sceneInner->getHeroHex());
        QPixmap unitTexture;

        if (isHeroOnHex) {
            if (hexInner->haveUnit()) {
                unitTexture = TextureManager::getInstance().getUnitTexture(hexInner->getUnit()->getType(), true);
            } else {
                unitTexture = TextureManager::getInstance().getUnitTexture(UnitType::MainHero, false);
            }
        } else if (hexInner->haveUnit()) {
            unitTexture = TextureManager::getInstance().getUnitTexture(hexInner->getUnit()->getType(), false);
        }

        if (!unitTexture.isNull()) {
            painter->save();
            painter->setClipPath(shape());
            painter->drawPixmap(-unitTexture.width()/2, -unitTexture.height()/2, unitTexture);

            if (!hexInner->visibilityState() && hexInner->explorationState()) {
                painter->setCompositionMode(QPainter::CompositionMode_SourceAtop);
                painter->fillRect(boundingRect(), QColor(0, 0, 0, 100));
            }
            painter->restore();
        }

        Unit* u = hexInner->getUnit();
        if (u && u->isEnemy() && (hexInner->visibilityState() || hexInner->explorationState())) {
            drawLevelBadge(painter, u->getLevel());
        }
    }

    if (option->state & QStyle::State_MouseOver) {
        if (!sceneInner->checkPanning()){
            if (hexInner->visibilityState() || hexInner->explorationState()) {
                painter->setBrush(QColor(255, 255, 255, 60));
                painter->setPen(Qt::NoPen);
                painter->drawPolygon(polygon());
            }
        }
    }

    Hex* heroHex = sceneInner->getHeroHex();
    QPen pen(Qt::black, 1);

    if (heroHex && heroHex->isNeighbor(*hexInner)) {
        bool isBlocked = false;
        if (hexInner->haveUnit() && hexInner->getUnit()->getType() == UnitType::StructUnBreak) {
            isBlocked = true;
        }

        if (isBlocked) {
            pen.setColor(Qt::red);
            pen.setWidthF(2.0);
        } else {
            pen.setColor(QColor(255, 215, 0));
            pen.setWidthF(2.0);
        }
    }
    painter->setPen(pen);
    painter->setBrush(Qt::NoBrush);
    painter->drawPolygon(polygon());

    if (isPathHighlighted)
    {
        QPen highlightPen(Qt::green);
        highlightPen.setWidth(2.0);
        painter->setPen(highlightPen);
        painter->setBrush(Qt::NoBrush);
        painter->drawPolygon(polygon());
    }
}

void HexItem::drawLevelBadge(QPainter* painter, int level)
{
    QString levelText = QString("Lvl %1").arg(level);

    QFont f = painter->font();
    f.setBold(true);
    f.setPointSize(14);
    painter->setFont(f);

    QPointF textPos(-15, -GlobalConst::HexSize / 2.0);

    painter->setPen(Qt::black);
    painter->drawText(textPos + QPointF(1, 1), levelText);

    painter->setPen(Qt::white);
    painter->drawText(textPos, levelText);
}

void HexItem::mousePressEvent(QGraphicsSceneMouseEvent* event)
{
    if (event->button() == Qt::LeftButton) {
        sceneInner->handleHexClick(this);
    }
    QGraphicsPolygonItem::mousePressEvent(event);
}

void HexItem::updateState()
{
    update();
}

void HexItem::updateZValue()
{
    qreal newZ = 0;

    if (isPathHighlighted) {
        newZ = 10;
    }
    else {
        Hex* heroHex = sceneInner->getHeroHex();
        if (heroHex && heroHex->isNeighbor(*hexInner)) {
            bool isBlocked = false;
            if (hexInner->haveUnit() && hexInner->getUnit()->getType() == UnitType::StructUnBreak) {
                isBlocked = true;
            }
            newZ = isBlocked ? 2 : 1;
        }
    }

    setZValue(newZ);
}

void HexItem::hoverEnterEvent(QGraphicsSceneHoverEvent* event)
{
    QGraphicsPolygonItem::hoverEnterEvent(event);
    update();
}

void HexItem::hoverLeaveEvent(QGraphicsSceneHoverEvent* event)
{
    QGraphicsPolygonItem::hoverLeaveEvent(event);
    update();
}

void HexItem::setPathHighlight(bool active)
{
    if (isPathHighlighted != active) {
        isPathHighlighted = active;
        updateZValue();
        update();
    }
}
