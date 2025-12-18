#include "GameView.h"
#include "GameScene.h"
#include <QWheelEvent>
#include <QScrollBar>

GameView::GameView(GameScene* scene, QWidget* parent)
    : QGraphicsView(scene, parent)
{
    setRenderHint(QPainter::Antialiasing);
    setViewportUpdateMode(QGraphicsView::SmartViewportUpdate);
    setHorizontalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setVerticalScrollBarPolicy(Qt::ScrollBarAlwaysOff);
    setTransformationAnchor(QGraphicsView::AnchorUnderMouse);
    setResizeAnchor(QGraphicsView::AnchorUnderMouse);
    setStyleSheet("background: transparent; border: 0px;");
    viewport()->setAttribute(Qt::WA_TranslucentBackground);
    if (scene) {
        scene->setBackgroundBrush(Qt::NoBrush);
    }
    setMinimumSize(800, 600);
    if (scene) {
        centerOn(scene->itemsBoundingRect().center());
    }
}

void GameView::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton) {
        isPanning = true;
        lastPanPos = event->pos();
        setCursor(Qt::ClosedHandCursor);

        if (GameScene* myScene = dynamic_cast<GameScene*>(scene())) {
            myScene->setPanning(true);
        }

        event->accept();
    } else {
        QGraphicsView::mousePressEvent(event);
    }
}

void GameView::mouseMoveEvent(QMouseEvent* event)
{
    if (isPanning) {
        QPoint delta = event->pos() - lastPanPos;
        lastPanPos = event->pos();

        horizontalScrollBar()->setValue(horizontalScrollBar()->value() - delta.x());
        verticalScrollBar()->setValue(verticalScrollBar()->value() - delta.y());
        event->accept();
    } else {
        QGraphicsView::mouseMoveEvent(event);
    }
}

void GameView::mouseReleaseEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton) {
        isPanning = false;
        setCursor(Qt::ArrowCursor);

        if (GameScene* myScene = dynamic_cast<GameScene*>(scene())) {
            myScene->setPanning(false);
        }

        event->accept();
    } else {
        QGraphicsView::mouseReleaseEvent(event);
    }
}

void GameView::wheelEvent(QWheelEvent* event)
{
    const double scaleFactor = 1.15;

    if (event->angleDelta().y() > 0) {
        scale(scaleFactor, scaleFactor);
    } else {
        scale(1.0 / scaleFactor, 1.0 / scaleFactor);
    }
}
