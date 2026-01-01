#include "GameSelectionWidget.h"

#include <QPushButton>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFrame>
#include <QGraphicsDropShadowEffect>

GameSelectionWidget::GameSelectionWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();
}

GameSelectionWidget::~GameSelectionWidget()
{
}

void GameSelectionWidget::setupUi()
{
    this->setAttribute(Qt::WA_StyledBackground, true);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);

    containerFrame = new QFrame(this);
    containerFrame->setFixedSize(550, 420);
    containerFrame->setObjectName("Container");

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0,0,0, 150));
    shadow->setOffset(0, 10);
    containerFrame->setGraphicsEffect(shadow);

    mainLayout->addWidget(containerFrame);

    QString styles = R"(
        QFrame#Container {
            background-color: #FFF9C4;
            border: 4px solid #FBC02D;
            border-radius: 20px;
        }

        QLabel {
            font-family: 'Unispace';
            font-size: 20pt;
            font-weight: bold;
            color: #3E2723;
        }

        QPushButton {
            background: transparent;
            border: none;
        }

        QPushButton#BtnNew {
            border-image: url(btn_newgame.png) 0 0 0 0 stretch stretch;
        }
        QPushButton#BtnLoad {
            border-image: url(btn_continue.png) 0 0 0 0 stretch stretch;
        }
        QPushButton#BtnBack {
            border-image: url(btn_return.png) 0 0 0 0 stretch stretch;
        }

        QPushButton:hover {
            margin-top: 2px;
        }
        QPushButton:pressed {
            margin-top: 5px;
        }
    )";
    this->setStyleSheet(styles);

    QVBoxLayout *frameLayout = new QVBoxLayout(containerFrame);
    frameLayout->setContentsMargins(40, 50, 40, 40);
    frameLayout->setSpacing(20);

    labelTitle = new QLabel("Start a new game?", containerFrame);
    labelTitle->setAlignment(Qt::AlignCenter);
    labelTitle->setWordWrap(true);
    frameLayout->addWidget(labelTitle);

    frameLayout->addStretch();

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(20);

    btnNewGame = new QPushButton(containerFrame);
    btnNewGame->setObjectName("BtnNew");
    btnNewGame->setFixedSize(150, 70);
    btnNewGame->setCursor(Qt::PointingHandCursor);

    btnLoadGame = new QPushButton(containerFrame);
    btnLoadGame->setObjectName("BtnLoad");
    btnLoadGame->setFixedSize(150, 70);
    btnLoadGame->setCursor(Qt::PointingHandCursor);

    buttonsLayout->addWidget(btnNewGame);
    buttonsLayout->addWidget(btnLoadGame);

    frameLayout->addLayout(buttonsLayout);

    frameLayout->addSpacing(10);

    btnBack = new QPushButton(containerFrame);
    btnBack->setObjectName("BtnBack");
    btnBack->setFixedSize(150, 70);
    btnBack->setCursor(Qt::PointingHandCursor);

    frameLayout->addWidget(btnBack, 0, Qt::AlignCenter);

    connect(btnNewGame, &QPushButton::clicked, this, &GameSelectionWidget::startNewGameClicked);
    connect(btnLoadGame, &QPushButton::clicked, this, &GameSelectionWidget::loadGameClicked);
    connect(btnBack, &QPushButton::clicked, this, &GameSelectionWidget::backClicked);
}
