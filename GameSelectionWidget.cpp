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

    m_containerFrame = new QFrame(this);
    m_containerFrame->setFixedSize(550, 420);
    m_containerFrame->setObjectName("Container");

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0,0,0, 150));
    shadow->setOffset(0, 10);
    m_containerFrame->setGraphicsEffect(shadow);

    mainLayout->addWidget(m_containerFrame);

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

    QVBoxLayout *frameLayout = new QVBoxLayout(m_containerFrame);
    frameLayout->setContentsMargins(40, 50, 40, 40);
    frameLayout->setSpacing(20);

    m_labelTitle = new QLabel("Start a new game?", m_containerFrame);
    m_labelTitle->setAlignment(Qt::AlignCenter);
    m_labelTitle->setWordWrap(true);
    frameLayout->addWidget(m_labelTitle);

    frameLayout->addStretch();

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(20);

    m_btnNewGame = new QPushButton(m_containerFrame);
    m_btnNewGame->setObjectName("BtnNew");
    m_btnNewGame->setFixedSize(150, 70);
    m_btnNewGame->setCursor(Qt::PointingHandCursor);

    m_btnLoadGame = new QPushButton(m_containerFrame);
    m_btnLoadGame->setObjectName("BtnLoad");
    m_btnLoadGame->setFixedSize(150, 70);
    m_btnLoadGame->setCursor(Qt::PointingHandCursor);

    buttonsLayout->addWidget(m_btnNewGame);
    buttonsLayout->addWidget(m_btnLoadGame);

    frameLayout->addLayout(buttonsLayout);

    frameLayout->addSpacing(10);

    m_btnBack = new QPushButton(m_containerFrame);
    m_btnBack->setObjectName("BtnBack");
    m_btnBack->setFixedSize(150, 70);
    m_btnBack->setCursor(Qt::PointingHandCursor);

    frameLayout->addWidget(m_btnBack, 0, Qt::AlignCenter);

    connect(m_btnNewGame, &QPushButton::clicked, this, &GameSelectionWidget::StartNewGameClicked);
    connect(m_btnLoadGame, &QPushButton::clicked, this, &GameSelectionWidget::LoadGameClicked);
    connect(m_btnBack, &QPushButton::clicked, this, &GameSelectionWidget::BackClicked);
}
