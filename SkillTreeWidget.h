#ifndef SKILLTREEWIDGET_H
#define SKILLTREEWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <vector>
#include "SkillTreeSystem.h"

class QLabel;
class MainHero;
class QPainter;
class QMouseEvent;
class QResizeEvent;
class QShowEvent;
class QKeyEvent;

class SkillNodeButton : public QPushButton {
public:
    std::string nodeId;
    SkillNode nodeData;

    SkillNodeButton(const SkillNode& data, QWidget* parent);
};

class SkillTreeWidget : public QWidget {
    Q_OBJECT

public:
    explicit SkillTreeWidget(MainHero* h, QWidget* parent = nullptr);

    void initializeTree();
    void updateUI();

private slots:
    void closeTree();
    void onNodeClicked(SkillNodeButton* btn);

signals:
    void closed();

protected:
    void showEvent(QShowEvent *event) override;
    void resizeEvent(QResizeEvent* event) override;
    void paintEvent(QPaintEvent* event) override;

    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void wheelEvent(QWheelEvent *event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    MainHero* hero;
    std::vector<SkillNodeButton*> buttons;

    QLabel* pointsLabel;
    QPushButton* closeBtn;
    QPushButton* centerBtn;

    const int RADIUS_STEP = 110;

    QColor getColorForType(SpellType type);
    double getAngleForType(SpellType type);
};

#endif // SKILLTREEWIDGET_H
