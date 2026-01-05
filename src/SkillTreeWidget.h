#ifndef SKILLTREEWIDGET_H
#define SKILLTREEWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QFrame>
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
    QPointF logicalPosition;

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
    void onUnlockBtnClicked();

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
    SkillNodeButton* selectedBtn;

    QLabel* pointsLabel;
    QPushButton* closeBtn;
    QPushButton* centerBtn;

    QFrame* infoPanel;
    QLabel* infoTitleLabel;
    QLabel* infoDescLabel;
    QLabel* infoCostLabel;
    QPushButton* infoUnlockBtn;

    double scaleFactor;
    QPoint panOffset;
    QPoint lastMousePos;
    bool isDragging;

    const int RADIUS_STEP = 130;

    void setupInfoPanel();
    void updateNodePositions();
    void updateInfoPanel();

    QColor getColorForType(SpellType type);
    double getAngleForType(SpellType type);
};

#endif // SKILLTREEWIDGET_H
