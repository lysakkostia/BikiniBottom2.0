#include "herowidget.h"
#include "Unit.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>

HeroWidget::HeroWidget(const QPixmap& Hero, GameScene* mappa, QWidget *parent)
    : QWidget(parent)
    , Mappa(mappa)
    , HeroTexture(Hero)
{
    setupUi();
    Update_stats();
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

    m_containerFrame = new QFrame(this);
    m_containerFrame->setObjectName("HeroFrame");
    mainLayout->addWidget(m_containerFrame);

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(15);
    shadow->setColor(QColor(0, 0, 0, 180));
    shadow->setOffset(2, 2);
    m_containerFrame->setGraphicsEffect(shadow);

    QHBoxLayout* hLayout = new QHBoxLayout(m_containerFrame);
    hLayout->setContentsMargins(10, 10, 10, 10);
    hLayout->setSpacing(10);

    m_avatarLabel = new QLabel(m_containerFrame);
    m_avatarLabel->setFixedSize(70, 70);
    m_avatarLabel->setAlignment(Qt::AlignCenter);
    m_avatarLabel->setStyleSheet("border: 2px solid #5D4037; border-radius: 5px; background-color: #3E2723;");

    if (!HeroTexture.isNull()) {
        m_avatarLabel->setPixmap(HeroTexture.scaled(m_avatarLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        m_avatarLabel->setText("?");
    }

    hLayout->addWidget(m_avatarLabel);

    QVBoxLayout* statsLayout = new QVBoxLayout();
    statsLayout->setSpacing(4);
    statsLayout->setContentsMargins(0, 2, 0, 2);

    m_levelLabel = new QLabel("Lvl 1", m_containerFrame);
    m_levelLabel->setObjectName("LevelLabel");
    m_levelLabel->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    statsLayout->addWidget(m_levelLabel);

    m_hpBar = new QProgressBar(m_containerFrame);
    m_hpBar->setObjectName("HPBar");
    m_hpBar->setFixedHeight(14);
    m_hpBar->setTextVisible(true);
    statsLayout->addWidget(m_hpBar);

    m_manaBar = new QProgressBar(m_containerFrame);
    m_manaBar->setObjectName("ManaBar");
    m_manaBar->setFixedHeight(14);
    m_manaBar->setTextVisible(true);
    statsLayout->addWidget(m_manaBar);

    m_xpBar = new QProgressBar(m_containerFrame);
    m_xpBar->setObjectName("XPBar");
    m_xpBar->setFixedHeight(8);
    m_xpBar->setTextVisible(false);
    statsLayout->addWidget(m_xpBar);

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

void HeroWidget::Update_stats()
{
    if (!Mappa) return;

    MainHero* hero = Mappa->GetHero();

    if (hero) {
        m_levelLabel->setText(QString("Lvl %1").arg(static_cast<int>(hero->GetLevel())));

        double currentHP = hero->GetHP();
        double maxHP = hero->GetMaxHP();
        m_hpBar->setRange(0, static_cast<int>(maxHP));
        m_hpBar->setValue(static_cast<int>(currentHP));
        m_hpBar->setFormat(QString("%1/%2").arg(static_cast<int>(currentHP)).arg(static_cast<int>(maxHP)));

        double currentMana = hero->GetMana();
        double maxMana = hero->GetMaxMana();
        m_manaBar->setRange(0, static_cast<int>(maxMana));
        m_manaBar->setValue(static_cast<int>(currentMana));
        m_manaBar->setFormat(QString("%1/%2").arg(static_cast<int>(currentMana)).arg(static_cast<int>(maxMana)));

        double currentXP = hero->GetCurrentXP();
        double maxXP = hero->GetMaxXP();

        m_xpBar->setRange(0, static_cast<int>(maxXP));
        m_xpBar->setValue(static_cast<int>(currentXP));

        m_xpBar->setToolTip(QString("XP: %1 / %2").arg(static_cast<int>(currentXP)).arg(static_cast<int>(maxXP)));
    }
}
