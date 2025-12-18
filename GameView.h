#ifndef GAMEVIEW_H
#define GAMEVIEW_H

#include <QGraphicsView>

class GameScene;

class GameView : public QGraphicsView
{
    Q_OBJECT

private:
    bool isPanning = false;
    QPoint lastPanPos;

public:
    GameView(GameScene* scene, QWidget* parent = nullptr);

protected:
    void mousePressEvent(QMouseEvent* event) override;
    void mouseMoveEvent(QMouseEvent* event) override;
    void mouseReleaseEvent(QMouseEvent* event) override;
    void wheelEvent(QWheelEvent* event) override;
};

#endif // GAMEVIEW_H
