#ifndef CAMPFIREWIDGET_H
#define CAMPFIREWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QMouseEvent>

class CampfireWidget : public QWidget {
    Q_OBJECT

public:
    explicit CampfireWidget(QWidget *parent = nullptr);

    void showRestDetails(double oldHP, double newHP, double oldMana, double newMana, int remainingCharges);

signals:
    void finished();

protected:
    void paintEvent(QPaintEvent *event) override;

    void mousePressEvent(QMouseEvent *event) override { event->accept(); }
    void mouseReleaseEvent(QMouseEvent *event) override { event->accept(); }

private:
    QLabel* titleLabel;
    QLabel* hpLabel;
    QLabel* manaLabel;
    QLabel* statusLabel;
    QPushButton* leaveBtn;

    QString formatStatChange(QString name, double oldVal, double newVal, QString color);
};

#endif // CAMPFIREWIDGET_H
