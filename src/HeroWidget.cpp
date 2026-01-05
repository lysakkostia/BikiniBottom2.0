#include "herowidget.h"
#include "Unit.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>

HeroWidget::HeroWidget(const QPixmap& Hero, GameScene* mappa, QWidget *parent)
    : QWidget(parent)
    , mapInner(mappa)
    , heroTexture(Hero)
{
    setupUi();
    updateStats();
}

HeroWidget::~HeroWidget()
{
}

void HeroWidget::setupUi()
{
    this->setFixedSize(280, 100);
    this->setAttribute(Qt::WA_StyledBackground, true);

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    containerFrame = new QFrame(this);
    containerFrame->setObjectName("HeroFrame");
    mainLayout->addWidget(containerFrame);

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 180));
    shadow->setOffset(2, 2);
    containerFrame->setGraphicsEffect(shadow);

    QHBoxLayout* hLayout = new QHBoxLayout(containerFrame);
    hLayout->setContentsMargins(10, 10, 10, 10);
    hLayout->setSpacing(10);

    avatarLabel = new QLabel(containerFrame);
    avatarLabel->setFixedSize(70, 70);
    avatarLabel->setAlignment(Qt::AlignCenter);
    avatarLabel->setStyleSheet("border: 2px solid #5D4037; border-radius: 5px; background-color: #3E2723;");

    if (!heroTexture.isNull()) {
        avatarLabel->setPixmap(heroTexture.scaled(avatarLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        avatarLabel->setText("?");
    }

    hLayout->addWidget(avatarLabel);

    QVBoxLayout* statsLayout = new QVBoxLayout();
    statsLayout->setSpacing(4);
    statsLayout->setContentsMargins(0, 2, 0, 2);

    levelLabel = new QLabel("Lvl 1", containerFrame);
    levelLabel->setObjectName("LevelLabel");
    levelLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    statsLayout->addWidget(levelLabel);

    hpBar = new QProgressBar(containerFrame);
    hpBar->setObjectName("HPBar");
    hpBar->setFixedHeight(14);
    hpBar->setTextVisible(true);
    statsLayout->addWidget(hpBar);

    manaBar = new QProgressBar(containerFrame);
    manaBar->setObjectName("ManaBar");
    manaBar->setFixedHeight(14);
    manaBar->setTextVisible(true);
    statsLayout->addWidget(manaBar);

    xpBar = new QProgressBar(containerFrame);
    xpBar->setObjectName("XPBar");
    xpBar->setFixedHeight(8);
    xpBar->setTextVisible(false);
    statsLayout->addWidget(xpBar);

    hLayout->addLayout(statsLayout);

    QString style = R"(
        QFrame#HeroFrame {
            background-color: rgba(40, 30, 20, 220);
            border: 2px solid #8D6E63;
            border-radius: 10px;
        }

        QLabel#LevelLabel {
            font-family: 'Unispace';
            font-size: 14pt;
            font-weight: bold;
            color: #FFECB3;
        }

        QProgressBar {
            border: 1px solid #3E2723;
            border-radius: 4px;
            background-color: #1a1a1a;
            text-align: center;
            color: white;
            font-size: 10px;
            font-weight: bold;
        }

        QProgressBar#HPBar::chunk {
            background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #D32F2F, stop:1 #E57373);
            border-radius: 3px;
        }

        QProgressBar#ManaBar::chunk {
            background-color: qlineargradient(x1:0, y1:0, x2:1, y2:0, stop:0 #1976D2, stop:1 #64B5F6);
            border-radius: 3px;
        }

        QProgressBar#XPBar {
            border: none;
            background-color: #424242;
            max-height: 6px;
        }
        QProgressBar#XPBar::chunk {
            background-color: #00BF00;
            border-radius: 3px;
        }
    )";
    this->setStyleSheet(style);
}

void HeroWidget::updateStats()
{
    if (!mapInner) return;

    MainHero* hero = mapInner->getHero();

    if (hero) {
        levelLabel->setText(QString("Lvl %1").arg(static_cast<int>(hero->getLevel())));

        double currentHP = hero->getHP();
        double maxHP = hero->getMaxHP();
        hpBar->setRange(0, static_cast<int>(maxHP));
        hpBar->setValue(static_cast<int>(currentHP));
        hpBar->setFormat(QString("%1/%2").arg(static_cast<int>(currentHP)).arg(static_cast<int>(maxHP)));

        double currentMana = hero->getMana();
        double maxMana = hero->getMaxMana();
        manaBar->setRange(0, static_cast<int>(maxMana));
        manaBar->setValue(static_cast<int>(currentMana));
        manaBar->setFormat(QString("%1/%2").arg(static_cast<int>(currentMana)).arg(static_cast<int>(maxMana)));

        double currentXP = hero->getCurrentXP();
        double maxXP = hero->getMaxXP();

        xpBar->setRange(0, static_cast<int>(maxXP));
        xpBar->setValue(static_cast<int>(currentXP));

        xpBar->setToolTip(QString("XP: %1 / %2").arg(static_cast<int>(currentXP)).arg(static_cast<int>(maxXP)));
    }
}
