#ifndef HEROWIDGET_H
#define HEROWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QProgressBar>
#include <QFrame>
#include "GameScene.h"

class HeroWidget : public QWidget
{
    Q_OBJECT

public:
    explicit HeroWidget(const QPixmap& Hero, GameScene* mappa, QWidget *parent = nullptr);
    ~HeroWidget();

    void updateStats();

private:
    void setupUi();

    GameScene* mapInner;
    QPixmap heroTexture;

    QFrame* containerFrame;
    QLabel* avatarLabel;
    QLabel* levelLabel;

    QProgressBar* hpBar;
    QProgressBar* manaBar;
    QProgressBar* xpBar;
};

#endif // HEROWIDGET_H
