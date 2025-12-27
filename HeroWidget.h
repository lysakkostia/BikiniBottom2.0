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

    void Update_stats();

private:
    void setupUi();

    GameScene* Mappa;
    QPixmap HeroTexture;

    QFrame* m_containerFrame;
    QLabel* m_avatarLabel;
    QLabel* m_levelLabel;

    QProgressBar* m_hpBar;
    QProgressBar* m_manaBar;
    QProgressBar* m_xpBar;
};

#endif // HEROWIDGET_H
