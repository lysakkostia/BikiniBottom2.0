#include "MainWindow.h"
#include <QUrl>
#include <QMessageBox>
#include <QDebug>
#include <QKeyEvent>
#include <QVBoxLayout>
#include <QLabel>
#include <QFrame>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , m_menuWidget(nullptr)
    , m_settingsWidget(nullptr)
    , MapRadius(10)
    , heroWidget(nullptr)
{
    this->setFixedSize(1280, 720);
    this->setMinimumSize(1280, 720);
    this->setWindowTitle("HexRPG");
    this->setWindowIcon(QIcon("icon.png"));
    this->setAutoFillBackground(false);

    this->setStyleSheet(
        "QMainWindow {"
        "    background-image: url(background.png);"
        "    background-repeat: no-repeat;"
        "    background-position: center;"
        "    background-attachment: fixed;"
        "    border-image: url(background.png) 0 0 0 0 stretch stretch;"
        "}"
        );

    m_stackedWidget = new QStackedWidget(this);
    this->setCentralWidget(m_stackedWidget);

    InitializeMenuUI();
    m_stackedWidget->addWidget(m_menuWidget);

    m_settingsWidget = new SettingsWidget(this);
    m_stackedWidget->addWidget(m_settingsWidget);

    m_gameSelectWidget = new GameSelectionWidget(this);
    m_stackedWidget->addWidget(m_gameSelectWidget);

    m_stackedWidget->setCurrentWidget(m_menuWidget);

    connect(m_settingsWidget, &SettingsWidget::MapRadChanged, this, &MainWindow::HandleMapRadiusChanged);
    connect(m_settingsWidget, &SettingsWidget::VolumeChanged, this, &MainWindow::HandleVolumeChanged);
    connect(m_settingsWidget, &SettingsWidget::BackClicked, this, &MainWindow::HandleBackToMenu);

    connect(m_gameSelectWidget, &GameSelectionWidget::StartNewGameClicked, this, &MainWindow::StartNewGame);
    connect(m_gameSelectWidget, &GameSelectionWidget::LoadGameClicked, this, &MainWindow::LoadSavedGame);
    connect(m_gameSelectWidget, &GameSelectionWidget::BackClicked, this, &MainWindow::HandleBackToMenu);

    connect(btn_play, &QPushButton::clicked, this, &MainWindow::on_btn_play_clicked);
    connect(btn_settings, &QPushButton::clicked, this, &MainWindow::on_btn_settings_clicked);
    connect(btn_exit, &QPushButton::clicked, this, &MainWindow::on_btn_exit_clicked);

    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    player->setSource(QUrl::fromLocalFile("adventure.mp3"));
    player->setLoops(QMediaPlayer::Infinite);
    audioOutput->setVolume(0);
    player->play();

    levelUpWidget = new LevelUpWidget(this);
    levelUpWidget->hide();
    connect(levelUpWidget, &LevelUpWidget::OptionSelected, this, &MainWindow::OnLevelUpOptionSelected);
}

MainWindow::~MainWindow()
{
    CleanupGame();
    delete player;
    delete audioOutput;
}

void MainWindow::InitializeMenuUI()
{
    m_menuWidget = new QWidget(this);
    m_menuWidget->setGeometry(0, 0, 1280, 720);

    btn_play = new QPushButton(m_menuWidget);
    btn_play->setObjectName("btn_play");
    btn_play->setGeometry(535, 300, 210, 80);
    QFont fontPlay;
    fontPlay.setPointSize(10);
    btn_play->setFont(fontPlay);
    btn_play->setIconSize(QSize(410, 300));
    btn_play->setStyleSheet(
        "QPushButton {"
        "    border-image: url(btn_play) 0 0 0 0 stretch stretch;"
        "    background: transparent;"
        "}"
        );

    btn_settings = new QPushButton(m_menuWidget);
    btn_settings->setObjectName("btn_settings");
    btn_settings->setGeometry(535, 400, 210, 90);
    btn_settings->setIconSize(QSize(410, 310));
    btn_settings->setStyleSheet(
        "QPushButton {"
        "    border-image: url(btn_options) 0 0 0 0 stretch stretch;"
        "    background: transparent;"
        "}"
        );

    btn_exit = new QPushButton(m_menuWidget);
    btn_exit->setObjectName("btn_exit");
    btn_exit->setGeometry(535, 500, 210, 90);
    btn_exit->setStyleSheet(
        "QPushButton {"
        "    border-image: url(btn_exit) 0 0 0 0 stretch stretch;"
        "    background: transparent;"
        "}"
        );

    lbl_title = new QLabel(m_menuWidget);
    lbl_title->setObjectName("label");
    lbl_title->setGeometry(490, 20, 300, 300);
    QFont fontTitle("Times New Roman", 14);
    fontTitle.setBold(true);
    lbl_title->setFont(fontTitle);
    lbl_title->setAlignment(Qt::AlignCenter);
    lbl_title->setStyleSheet(
        "QLabel {"
        "    border: none;"
        "    border-image: url(icon) 0 0 0 0 stretch stretch;"
        "    background: transparent;"
        "}"
        );

    lbl_footer = new QLabel(m_menuWidget);
    lbl_footer->setObjectName("label_2");
    lbl_footer->setGeometry(-20, 670, 421, 71);
    QFont fontFooter("Unispace", 17);
    fontFooter.setBold(true);
    lbl_footer->setFont(fontFooter);
    lbl_footer->setText("Made by BikiniBottom Group");
    lbl_footer->setAlignment(Qt::AlignCenter);

    btn_settings->raise();
    btn_exit->raise();
    lbl_title->raise();
    btn_play->raise();
    lbl_footer->raise();
}

//радіус гри
void MainWindow::HandleMapRadiusChanged(int NewRadius)
{
    if(NewRadius > 0)
        MapRadius = NewRadius;
}

//кнопка виходу
void MainWindow::on_btn_exit_clicked()
{
    this->close();
}

//кнопка налаштування
void MainWindow::on_btn_settings_clicked()
{
    m_settingsWidget->SetCurrentRadius(MapRadius);
    m_stackedWidget->setCurrentWidget(m_settingsWidget);
}

void MainWindow::HandleBackToMenu()
{
    if (m_pauseWidget) m_pauseWidget->hide();
    m_stackedWidget->setCurrentWidget(m_menuWidget);
    if(MapView) {
        CleanupGame();
    }
}

void MainWindow::HandleVolumeChanged(int volume)
{
    if(audioOutput)
    {
        audioOutput->setVolume(volume/100.0);
    }
}

//кнопка гри
void MainWindow::on_btn_play_clicked()
{
    m_stackedWidget->setCurrentWidget(m_gameSelectWidget);
}

void MainWindow::StartNewGame()
{
    CleanupGame();

    MGameScene = new GameScene(MapRadius, this);
    MapView = new GameView(MGameScene, this);

    m_stackedWidget->addWidget(MapView);
    m_stackedWidget->setCurrentWidget(MapView);
    MapView->setFocus();

    MainHero* hero = MGameScene->GetHero();
    skillTreeWidget = new SkillTreeWidget(hero, this);
    skillTreeWidget->hide();

    SetupPauseWidget();

    m_treeBtn = new QPushButton(this);
    m_treeBtn->setIcon(QIcon("icon.png"));
    m_treeBtn->setGeometry(10, height() - 150, 50, 50);
    m_treeBtn->show();
    m_treeBtn->raise();

    connect(m_treeBtn, &QPushButton::clicked, [this]() {
        if (MGameScene) MGameScene->SetPaused(true);
        if (skillTreeWidget) skillTreeWidget->show();
    });

    connect(skillTreeWidget, &SkillTreeWidget::closed, [this]() {
        if (MGameScene) MGameScene->SetPaused(false);
    });

    m_campfireWidget = new CampfireWidget(this);
    m_campfireWidget->hide();

    m_npcWidget = new NPCWidget(this);
    m_npcWidget->hide();

    connect(MGameScene, &GameScene::npcInteractionRequested, this, &MainWindow::OnNPCInteractionRequested);
    connect(MGameScene, &GameScene::combatRequested, this, &MainWindow::OnCombatRequested);
    connect(MGameScene, &GameScene::campfireRequested, this, &MainWindow::OnCampfireRequested);
    connect(MGameScene, &GameScene::combatStarted, this, &MainWindow::OnCombatStarted);
    connect(MGameScene, &GameScene::combatEnded, this, &MainWindow::OnCombatEnded);
    connect(MGameScene, &GameScene::combatStarted, this, &MainWindow::OnCombatStarted);
    connect(MGameScene, &GameScene::combatEnded, this, &MainWindow::OnCombatEnded);
    connect(MGameScene, &GameScene::gameOver, this, &MainWindow::HandleGameOver);
    connect(MGameScene, &GameScene::victory, this, &MainWindow::HandleVictory);
    connect(MGameScene, &GameScene::levelUpTriggered, this, &MainWindow::HandleLevelUp);

    QPixmap HeroTexture("NPC5Texture.png");
    if (!HeroTexture.isNull()) {
        heroWidget = new HeroWidget(HeroTexture, MGameScene, this);
        heroWidget->setFixedSize(200, 100);
        heroWidget->move(10, height() - heroWidget->height() - 10);
        heroWidget->raise();
        heroWidget->show();

        connect(MGameScene, &GameScene::heroStatsChanged, heroWidget, &HeroWidget::Update_stats);
    }
}

void MainWindow::LoadSavedGame()
{
    CleanupGame();

    MGameScene = new GameScene(MapRadius, this);

    if (!MGameScene->LoadMapFromFile("map.dat")) {
        QMessageBox::warning(this, tr("Помилка"), tr("Не вдалося завантажити збереження."));
        delete MGameScene;
        MGameScene = nullptr;
        return;
    }

    MapView = new GameView(MGameScene, this);

    m_stackedWidget->addWidget(MapView);
    m_stackedWidget->setCurrentWidget(MapView);
    MapView->setFocus();

    MainHero* hero = MGameScene->GetHero();
    skillTreeWidget = new SkillTreeWidget(hero, this);
    skillTreeWidget->hide();

    SetupPauseWidget();

    m_treeBtn = new QPushButton(this);
    m_treeBtn->setIcon(QIcon("icon.png"));
    m_treeBtn->setGeometry(10, height() - 150, 50, 50);
    m_treeBtn->show();
    m_treeBtn->raise();

    connect(m_treeBtn, &QPushButton::clicked, [this]() {
        if (MGameScene) MGameScene->SetPaused(true);
        if (skillTreeWidget) skillTreeWidget->show();
    });

    connect(skillTreeWidget, &SkillTreeWidget::closed, [this]() {
        if (MGameScene) MGameScene->SetPaused(false);
    });

    m_campfireWidget = new CampfireWidget(this);
    m_campfireWidget->hide();

    m_npcWidget = new NPCWidget(this);
    m_npcWidget->hide();

    connect(MGameScene, &GameScene::npcInteractionRequested, this, &MainWindow::OnNPCInteractionRequested);
    connect(MGameScene, &GameScene::combatRequested, this, &MainWindow::OnCombatRequested);
    connect(MGameScene, &GameScene::campfireRequested, this, &MainWindow::OnCampfireRequested);
    connect(MGameScene, &GameScene::combatStarted, this, &MainWindow::OnCombatStarted);
    connect(MGameScene, &GameScene::combatEnded, this, &MainWindow::OnCombatEnded);
    connect(MGameScene, &GameScene::gameOver, this, &MainWindow::HandleGameOver);
    connect(MGameScene, &GameScene::victory, this, &MainWindow::HandleVictory);
    connect(MGameScene, &GameScene::levelUpTriggered, this, &MainWindow::HandleLevelUp);

    QPixmap HeroTexture("NPC5Texture.png");
    if (!HeroTexture.isNull()) {
        heroWidget = new HeroWidget(HeroTexture, MGameScene, this);
        heroWidget->setFixedSize(200, 100);
        heroWidget->move(10, height() - heroWidget->height() - 10);
        heroWidget->raise();
        heroWidget->show();

        connect(MGameScene, &GameScene::heroStatsChanged, heroWidget, &HeroWidget::Update_stats);
        heroWidget->Update_stats();
    }
}

void MainWindow::SetupPauseWidget()
{
    m_pauseWidget = new PauseWidget(this);
    m_pauseWidget->hide();

    m_pauseWidget->resize(this->size());

    connect(m_pauseWidget, &PauseWidget::ContinueClicked, this, &MainWindow::OnPauseContinue);
    connect(m_pauseWidget, &PauseWidget::ExitClicked, this, &MainWindow::OnPauseExit);
}

void MainWindow::CleanupGame()
{
    if (MapView) {
        m_stackedWidget->removeWidget(MapView);
        MapView->deleteLater();
        MapView = nullptr;
    }
    if (MGameScene) {
        MGameScene->deleteLater();
        MGameScene = nullptr;
    }
    if (heroWidget) {
        heroWidget->deleteLater();
        heroWidget = nullptr;
    }
    if (skillTreeWidget) {
        skillTreeWidget->deleteLater();
        skillTreeWidget = nullptr;
    }
    if (m_treeBtn) {
        m_treeBtn->deleteLater();
        m_treeBtn = nullptr;
    }
    if (m_pauseWidget) {
        m_pauseWidget->deleteLater();
        m_pauseWidget = nullptr;
    }
    if (m_fightWidget) {
        m_fightWidget->deleteLater();
        m_fightWidget = nullptr;
    }
    if (m_campfireWidget) {
        m_campfireWidget->deleteLater();
        m_campfireWidget = nullptr;
    }
    if (m_npcWidget) {
        m_npcWidget->deleteLater();
        m_npcWidget = nullptr;
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        on_btn_pause_clicked();
    }
    else
    {
        QMainWindow::keyPressEvent(event);
    }
}

//кнопка паузи
void MainWindow::on_btn_pause_clicked()
{
    if (!MGameScene) return;

    MGameScene->SetPaused(true);
    m_pauseWidget->resize(this->size());
    m_pauseWidget->raise();
    m_pauseWidget->show();
}

void MainWindow::OnPauseContinue()
{
    if (m_pauseWidget) m_pauseWidget->hide();
    if (MGameScene) MGameScene->SetPaused(false);
    if (MapView) MapView->setFocus();
}

void MainWindow::OnPauseExit()
{
    if (MGameScene) {
        MGameScene->SaveMapToFile("map.dat");
    }
    CleanupGame();
    m_stackedWidget->setCurrentWidget(m_menuWidget);
}

//подія програшу
void MainWindow::HandleGameOver()
{
    showEndGameDialog(tr("Гру завершено"), tr("На жаль, ваш герой загинув..."), false);
}

//подія перемоги
void MainWindow::HandleVictory()
{
    showEndGameDialog(tr("ПЕРЕМОГА!"), tr("Вітаємо! Ви успішно пройшли гру!"), true);
}

void MainWindow::HandleLevelUp()
{
    if (!MGameScene) return;

    MGameScene->SetPaused(true);

    std::vector<UpgradeOption> options = LevelUpGenerator::GenerateOptions();

    levelUpWidget->ShowOptions(options);

    int x = (this->width() - levelUpWidget->width()) / 2;
    int y = (this->height() - levelUpWidget->height()) / 2;
    levelUpWidget->move(x, y);

    levelUpWidget->raise();
    levelUpWidget->show();
}

void MainWindow::OnLevelUpOptionSelected(int index)
{
    if (!MGameScene) return;

    MainHero* hero = MGameScene->GetHero();

    UpgradeOption option = levelUpWidget->GetOption(index);
    hero->ApplyUpgrade(option);

    hero->DecrementLevelUpPending();

    if (hero->IsLevelUpPending()) {
        std::vector<UpgradeOption> newOptions = LevelUpGenerator::GenerateOptions();
        levelUpWidget->ShowOptions(newOptions);

        return;
    }

    MGameScene->SetPaused(false);
    if (MapView) {
        MapView->setFocus();
    }
}

void MainWindow::OnCombatStarted()
{
    if (heroWidget) heroWidget->hide();
    if (m_treeBtn) m_treeBtn->hide();

    if (skillTreeWidget) skillTreeWidget->hide();
}

void MainWindow::OnCombatEnded()
{
    if (heroWidget) heroWidget->show();
    if (m_treeBtn) m_treeBtn->show();
}

void MainWindow::OnCombatRequested(Unit* enemy)
{
    QPixmap heroTex = TextureManager::GetInstance().getUnitTexture(UnitType::MainHero);
    QPixmap enemyTex = TextureManager::GetInstance().getUnitTexture(enemy->GetType());

    MainHero* hero = MGameScene->GetHero();

    m_fightWidget = new Fight(heroTex, enemyTex, hero, enemy, this);
    m_fightWidget->resize(this->size());
    m_fightWidget->show();
    m_fightWidget->setFocus();

    connect(m_fightWidget, &Fight::battleEnded, [this, enemy](bool playerWon) {

        bool escaped = m_fightWidget->didPlayerEscaped();

        if (MGameScene) {
            MGameScene->FinishCombat(playerWon, escaped, enemy);
        }

        m_fightWidget->deleteLater();
        m_fightWidget = nullptr;
    });
}

void MainWindow::OnCampfireRequested(double oldHP, double newHP, double oldMana, double newMana, int charges, Unit* campfireUnit)
{
    if (!m_campfireWidget) return;

    m_campfireWidget->ShowRestDetails(oldHP, newHP, oldMana, newMana, charges);
    m_campfireWidget->raise();

    m_campfireWidget->disconnect(SIGNAL(finished()));

    connect(m_campfireWidget, &CampfireWidget::finished, [this, campfireUnit]() {
        if (MGameScene) {
            MGameScene->FinishCampfireInteraction(campfireUnit);
        }
    });
}

void MainWindow::OnNPCInteractionRequested(Unit* npcUnit, const QString& text)
{
    if (!m_npcWidget) return;

    QString name = "Friendly Villager";

    m_npcWidget->ShowDialogue(name, text);

    m_npcWidget->disconnect(SIGNAL(finished()));

    connect(m_npcWidget, &NPCWidget::finished, [this]() {
        if (MGameScene) {
            MGameScene->FinishNPCInteraction();
        }
    });
}

void MainWindow::showEndGameDialog(const QString& title, const QString& message, bool isVictory)
{
    if (MGameScene) {
        MGameScene->SetPaused(true);
    }

    QWidget* overlay = new QWidget(this);
    overlay->setGeometry(rect());
    overlay->setStyleSheet("background-color: rgba(0, 0, 0, 200);");
    overlay->setAttribute(Qt::WA_DeleteOnClose);

    QVBoxLayout* layout = new QVBoxLayout(overlay);
    layout->setAlignment(Qt::AlignCenter);

    QFrame* dialogFrame = new QFrame(overlay);
    dialogFrame->setFixedSize(400, 250);

    QString borderColor = isVictory ? "#FFD54F" : "#EF5350";

    dialogFrame->setStyleSheet(QString(
                                   "QFrame {"
                                   "   background-color: #3E2723;"
                                   "   border: 4px solid %1;"
                                   "   border-radius: 15px;"
                                   "}"
                                   ).arg(borderColor));

    QVBoxLayout* frameLayout = new QVBoxLayout(dialogFrame);
    frameLayout->setSpacing(20);
    frameLayout->setContentsMargins(30, 30, 30, 30);

    QLabel* lblTitle = new QLabel(title, dialogFrame);
    lblTitle->setAlignment(Qt::AlignCenter);
    lblTitle->setStyleSheet(QString("font-size: 28px; font-weight: bold; color: %1; border: none; background: transparent;").arg(borderColor));

    QLabel* lblMsg = new QLabel(message, dialogFrame);
    lblMsg->setAlignment(Qt::AlignCenter);
    lblMsg->setWordWrap(true);
    lblMsg->setStyleSheet("font-size: 16px; color: #E0E0E0; border: none; background: transparent;");

    QPushButton* btnMenu = new QPushButton(tr("У ГОЛОВНЕ МЕНЮ"), dialogFrame);
    btnMenu->setCursor(Qt::PointingHandCursor);
    btnMenu->setFixedSize(200, 50);
    btnMenu->setStyleSheet(
        "QPushButton {"
        "   background-color: #5D4037;"
        "   color: white;"
        "   font-weight: bold;"
        "   border: 2px solid #8D6E63;"
        "   border-radius: 10px;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover { background-color: #6D4C41; border-color: #FFD54F; }"
        "QPushButton:pressed { background-color: #3E2723; }"
        );

    connect(btnMenu, &QPushButton::clicked, [this, overlay]() {
        overlay->close();
        CleanupGame();
        m_stackedWidget->setCurrentWidget(m_menuWidget);
    });

    frameLayout->addWidget(lblTitle);
    frameLayout->addWidget(lblMsg);
    frameLayout->addWidget(btnMenu, 0, Qt::AlignCenter);

    layout->addWidget(dialogFrame);
    overlay->show();
    overlay->raise();
}
