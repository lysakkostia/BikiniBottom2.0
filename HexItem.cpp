#include "HexItem.h"
#include "GameScene.h"
#include "TextureManager.h"
#include <QPen>
#include <QStyleOptionGraphicsItem>
#include <QStyle>

HexItem::HexItem(Hex* modelHex, GameScene* scene, QGraphicsItem* parent)
    : QGraphicsPolygonItem(parent), MHex(modelHex), MScene(scene)
{
    createPolygon();
    setAcceptHoverEvents(true);
    QPointF center = MHex->GetCenter();
    setPos(center);
}

void HexItem::createPolygon()
{
    QPolygonF polygon;
    for (const auto& corner : MHex->GetCorners()) {
        polygon << (corner - MHex->GetCenter());
    }
    setPolygon(polygon);
}

void HexItem::paint(QPainter* painter, const QStyleOptionGraphicsItem* option, QWidget* widget)
{
    Q_UNUSED(option);
    Q_UNUSED(widget);

    QPixmap bgTexture = TextureManager::GetInstance().getTerrainTexture(MHex->VisibilityState(), MHex->ExplorationState());

    painter->save();
    painter->setClipPath(shape());

    if (!bgTexture.isNull()) {
        painter->drawPixmap(-bgTexture.width()/2, -bgTexture.height()/2, bgTexture);
    } else {
        QColor color = Qt::black;
        if (MHex->VisibilityState()) color = Qt::white;
        else if (MHex->ExplorationState()) color = Qt::darkGray;
        painter->setBrush(color);
        painter->drawPolygon(polygon());
    }
    painter->restore();

    if (MHex->VisibilityState() || MHex->ExplorationState())
    {
        bool isHeroOnHex = (MHex == MScene->getHeroHex());
        QPixmap unitTexture;

        if (isHeroOnHex) {
            if (MHex->HaveUnit()) {
                unitTexture = TextureManager::GetInstance().getUnitTexture(MHex->GetUnit()->GetType(), true);
            } else {
                unitTexture = TextureManager::GetInstance().getUnitTexture(UnitType::MainHero, false);
            }
        } else if (MHex->HaveUnit()) {
            unitTexture = TextureManager::GetInstance().getUnitTexture(MHex->GetUnit()->GetType(), false);
        }

        if (!unitTexture.isNull()) {
            painter->save();
            painter->setClipPath(shape());
            painter->drawPixmap(-unitTexture.width()/2, -unitTexture.height()/2, unitTexture);

            if (!MHex->VisibilityState() && MHex->ExplorationState()) {
                painter->setCompositionMode(QPainter::CompositionMode_SourceAtop);
                painter->fillRect(boundingRect(), QColor(0, 0, 0, 100));
            }
            painter->restore();
        }

        Unit* u = MHex->GetUnit();
        if (u && u->IsEnemy() && (MHex->VisibilityState() || MHex->ExplorationState())) {
            drawLevelBadge(painter, u->GetLevel());
        }
    }

    if (option->state & QStyle::State_MouseOver) {
        if (!MScene->isPanning()){
            if (MHex->VisibilityState() || MHex->ExplorationState()) {
                painter->setBrush(QColor(255, 255, 255, 60));
                painter->setPen(Qt::NoPen);
                painter->drawPolygon(polygon());
            }
        }
    }

    Hex* heroHex = MScene->getHeroHex();
    QPen pen(Qt::black, 1);

    if (heroHex && heroHex->IsNeighbor(*MHex)) {
        bool isBlocked = false;
        if (MHex->HaveUnit() && MHex->GetUnit()->GetType() == UnitType::StructUnBreak) {
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
        MScene->handleHexClick(this);
    }
    QGraphicsPolygonItem::mousePressEvent(event);
}

void HexItem::updateState()
{
    update();
}

void HexItem::updateZValue()
{
    Hex* heroHex = MScene->getHeroHex();

    qreal newZ = 0;

    if (heroHex && heroHex->IsNeighbor(*MHex)) {
        bool isBlocked = false;
        if (MHex->HaveUnit() && MHex->GetUnit()->GetType() == UnitType::StructUnBreak) {
            isBlocked = true;
        }

        if (isBlocked) {
            newZ = 2;
        } else {
            newZ = 1;
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
