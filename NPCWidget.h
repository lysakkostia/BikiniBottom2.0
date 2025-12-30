#ifndef NPCWIDGET_H
#define NPCWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>

class NPCWidget : public QWidget {
    Q_OBJECT

public:
    explicit NPCWidget(QWidget *parent = nullptr);

    void ShowDialogue(const QString& unitName, const QString& dialogueText);

signals:
    void finished();

protected:
    void paintEvent(QPaintEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override { event->accept(); }
    void mouseReleaseEvent(QMouseEvent *event) override { event->accept(); }

private:
    QLabel* nameLabel;
    QLabel* textLabel;
    QPushButton* closeBtn;
    QWidget* container;
};

#endif // NPCWIDGET_H
