#include "herowidget.h"
#include "ui_herowidget.h"
#include <QLabel>
#include <QResizeEvent>
#include <QVBoxLayout>
#include <QDebug>
#include <QTimer>
#include <QMessageBox>
#include <QListWidget>

HeroWidget::HeroWidget(const QPixmap& Hero, GameScene* mappa, QWidget *parent)
    : QWidget(parent)
    , ui(new Ui::HeroWidget)
    , Mappa(mappa)
    , HeroTexture(Hero)
{
    ui->setupUi(this);

    if (ui->HeroImageLabel) {
        ui->HeroImageLabel->setAlignment(Qt::AlignCenter);
        if (!HeroTexture.isNull()) {
            QPixmap scaled = HeroTexture.scaled(
                ui->HeroImageLabel->size(),
                Qt::KeepAspectRatio,
                Qt::SmoothTransformation
                );
            ui->HeroImageLabel->setPixmap(scaled);
        } else {
            QPixmap placeholder(200, 150);
            placeholder.fill(Qt::gray);
            ui->HeroImageLabel->setPixmap(placeholder);
            qWarning("Hero texture is null, showing placeholder.");
        }
    } else {
        qWarning("ui->HeroImageLabel is null. Please define it in .ui file.");
        QLabel* manualLabel = new QLabel(this);
        manualLabel->setAlignment(Qt::AlignCenter);
        if (!HeroTexture.isNull()) {
            manualLabel->setPixmap(HeroTexture);
        }
        if (layout()) {
            layout()->addWidget(manualLabel);
        } else {
            QVBoxLayout* fallbackLayout = new QVBoxLayout(this);
            this->setLayout(fallbackLayout);
            fallbackLayout->addWidget(manualLabel);
        }
    }

    displayStats();
}

HeroWidget::~HeroWidget()
{
    delete ui;
}

void HeroWidget::displayStats()
{
    GameScene::HeroStats stats;
    stats = Mappa->GetStats();
    if (stats.HP) {
        ui->Hero_HP->setText(tr("HP: %1").arg(stats.HP));
    } else {
        qWarning("ui->Hero_HP is null. Please define it in .ui file.");
    }
    if (stats.MP) {
        ui->Hero_Mana->setText(tr("MP: %1").arg(stats.MP));
    } else {
        qWarning("ui->Hero_HP is null. Please define it in .ui file.");
    }
    if (stats.LVL) {
        ui->Hero_Level->setText(tr("LVL: %1").arg(stats.LVL));
    } else {
        qWarning("ui->Hero_HP is null. Please define it in .ui file.");
    }
}

void HeroWidget::Update_stats()
{
    displayStats();
}
