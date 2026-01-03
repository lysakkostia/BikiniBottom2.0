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
    , menuWidget(nullptr)
    , settingsWidget(nullptr)
    , mapRadius(10)
    , heroWidget(nullptr)
{
    this->setFixedSize(1280, 720);
    this->setMinimumSize(1280, 720);
    this->setWindowTitle("HexRPG");
    this->setWindowIcon(QIcon(":/textures/icon.png"));
    this->setAutoFillBackground(false);

    this->setStyleSheet(
        "QMainWindow {"
        "    border-image: url(:/textures/background_main.png) 0 0 0 0 stretch stretch;"
        "}"
    );

    stackedWidget = new QStackedWidget(this);
    this->setCentralWidget(stackedWidget);

    initializeMenuUI();
    stackedWidget->addWidget(menuWidget);

    settingsWidget = new SettingsWidget(this);
    stackedWidget->addWidget(settingsWidget);

    gameSelectWidget = new GameSelectionWidget(this);
    stackedWidget->addWidget(gameSelectWidget);

    stackedWidget->setCurrentWidget(menuWidget);

    connect(settingsWidget, &SettingsWidget::mapRadChanged, this, &MainWindow::handleMapRadiusChanged);
    connect(settingsWidget, &SettingsWidget::volumeChanged, this, &MainWindow::handleVolumeChanged);
    connect(settingsWidget, &SettingsWidget::backClicked, this, &MainWindow::handleBackToMenu);

    connect(gameSelectWidget, &GameSelectionWidget::startNewGameClicked, this, &MainWindow::startNewGame);
    connect(gameSelectWidget, &GameSelectionWidget::loadGameClicked, this, &MainWindow::loadSavedGame);
    connect(gameSelectWidget, &GameSelectionWidget::backClicked, this, &MainWindow::handleBackToMenu);

    connect(btnPlay, &QPushButton::clicked, this, &MainWindow::onBtnPlayClicked);
    connect(btnSettings, &QPushButton::clicked, this, &MainWindow::onBtnSettingsClicked);
    connect(btnExit, &QPushButton::clicked, this, &MainWindow::onBtnExitClicked);

    mediaPlayer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    mediaPlayer->setAudioOutput(audioOutput);
    mediaPlayer->setSource(QUrl("qrc:/sounds/background_music.mp3"));
    mediaPlayer->setLoops(QMediaPlayer::Infinite);
    audioOutput->setVolume(0);
    mediaPlayer->play();

    levelUpWidget = new LevelUpWidget(this);
    levelUpWidget->hide();
    connect(levelUpWidget, &LevelUpWidget::optionSelected, this, &MainWindow::onLevelUpOptionSelected);
}

MainWindow::~MainWindow()
{
    cleanupGame();
    delete mediaPlayer;
    delete audioOutput;
}

void MainWindow::initializeMenuUI()
{
    menuWidget = new QWidget(this);
    menuWidget->setGeometry(0, 0, 1280, 720);

    btnPlay = new QPushButton(menuWidget);
    btnPlay->setObjectName("btn_play");
    btnPlay->setGeometry(535, 300, 210, 80);
    QFont fontPlay;
    fontPlay.setPointSize(10);
    btnPlay->setFont(fontPlay);
    btnPlay->setIconSize(QSize(410, 300));
    btnPlay->setStyleSheet(
        "QPushButton {"
        "    border-image: url(:/textures/btn_play) 0 0 0 0 stretch stretch;"
        "    background: transparent;"
        "}"
        );

    btnSettings = new QPushButton(menuWidget);
    btnSettings->setObjectName("btn_settings");
    btnSettings->setGeometry(535, 400, 210, 90);
    btnSettings->setIconSize(QSize(410, 310));
    btnSettings->setStyleSheet(
        "QPushButton {"
        "    border-image: url(:/textures/btn_options) 0 0 0 0 stretch stretch;"
        "    background: transparent;"
        "}"
        );

    btnExit = new QPushButton(menuWidget);
    btnExit->setObjectName("btn_exit");
    btnExit->setGeometry(535, 500, 210, 90);
    btnExit->setStyleSheet(
        "QPushButton {"
        "    border-image: url(:/textures/btn_exit) 0 0 0 0 stretch stretch;"
        "    background: transparent;"
        "}"
        );

    lblTitle = new QLabel(menuWidget);
    lblTitle->setObjectName("label");
    lblTitle->setGeometry(490, 20, 300, 300);
    QFont fontTitle("Times New Roman", 14);
    fontTitle.setBold(true);
    lblTitle->setFont(fontTitle);
    lblTitle->setAlignment(Qt::AlignCenter);
    lblTitle->setStyleSheet(
        "QLabel {"
        "    border: none;"
        "    border-image: url(:/textures/icon) 0 0 0 0 stretch stretch;"
        "    background: transparent;"
        "}"
        );

    lblFooter = new QLabel(menuWidget);
    lblFooter->setObjectName("label_2");
    lblFooter->setGeometry(-20, 670, 421, 71);
    QFont fontFooter("Unispace", 17);
    fontFooter.setBold(true);
    lblFooter->setFont(fontFooter);
    lblFooter->setText("Made by BikiniBottom Group");
    lblFooter->setAlignment(Qt::AlignCenter);

    btnSettings->raise();
    btnExit->raise();
    lblTitle->raise();
    btnPlay->raise();
    lblFooter->raise();
}

//радіус гри
void MainWindow::handleMapRadiusChanged(int NewRadius)
{
    if(NewRadius > 0)
        mapRadius = NewRadius;
}

//кнопка виходу
void MainWindow::onBtnExitClicked()
{
    this->close();
}

//кнопка налаштування
void MainWindow::onBtnSettingsClicked()
{
    settingsWidget->setCurrentRadius(mapRadius);
    stackedWidget->setCurrentWidget(settingsWidget);
}

void MainWindow::handleBackToMenu()
{
    if (pauseWidget) pauseWidget->hide();
    stackedWidget->setCurrentWidget(menuWidget);
    if(mapView) {
        cleanupGame();
    }
}

void MainWindow::handleVolumeChanged(int volume)
{
    if(audioOutput)
    {
        audioOutput->setVolume(volume/100.0);
    }
}

//кнопка гри
void MainWindow::onBtnPlayClicked()
{
    stackedWidget->setCurrentWidget(gameSelectWidget);
}

void MainWindow::startNewGame()
{
    cleanupGame();

    gameScene = new GameScene(mapRadius, this);
    mapView = new GameView(gameScene, this);

    stackedWidget->addWidget(mapView);
    stackedWidget->setCurrentWidget(mapView);
    mapView->setFocus();

    MainHero* hero = gameScene->getHero();
    skillTreeWidget = new SkillTreeWidget(hero, this);
    skillTreeWidget->hide();

    setupPauseWidget();

    btnTree = new QPushButton(this);
    btnTree->setIcon(QIcon(":/textures/icon.png"));
    btnTree->setGeometry(10, height() - 150, 50, 50);
    btnTree->show();
    btnTree->raise();

    connect(btnTree, &QPushButton::clicked, [this]() {
        if (gameScene) gameScene->setPaused(true);
        if (skillTreeWidget) skillTreeWidget->show();
    });

    connect(skillTreeWidget, &SkillTreeWidget::closed, [this]() {
        if (gameScene) gameScene->setPaused(false);
    });

    campfireWidget = new CampfireWidget(this);
    campfireWidget->hide();

    npcWidget = new NPCWidget(this);
    npcWidget->hide();

    connect(gameScene, &GameScene::npcInteractionRequested, this, &MainWindow::onNPCInteractionRequested);
    connect(gameScene, &GameScene::combatRequested, this, &MainWindow::onCombatRequested);
    connect(gameScene, &GameScene::campfireRequested, this, &MainWindow::onCampfireRequested);
    connect(gameScene, &GameScene::combatStarted, this, &MainWindow::onCombatStarted);
    connect(gameScene, &GameScene::combatEnded, this, &MainWindow::onCombatEnded);
    connect(gameScene, &GameScene::combatStarted, this, &MainWindow::onCombatStarted);
    connect(gameScene, &GameScene::combatEnded, this, &MainWindow::onCombatEnded);
    connect(gameScene, &GameScene::gameOver, this, &MainWindow::handleGameOver);
    connect(gameScene, &GameScene::victory, this, &MainWindow::handleVictory);
    connect(gameScene, &GameScene::levelUpTriggered, this, &MainWindow::handleLevelUp);

    heroWidget = new HeroWidget(TextureManager::getInstance().getUnitTexture(UnitType::MainHero, false), gameScene, this);
    heroWidget->setFixedSize(200, 100);
    heroWidget->move(10, height() - heroWidget->height() - 10);
    heroWidget->raise();
    heroWidget->show();

    connect(gameScene, &GameScene::heroStatsChanged, heroWidget, &HeroWidget::updateStats);
}

void MainWindow::loadSavedGame()
{
    cleanupGame();

    gameScene = new GameScene(mapRadius, this);

    if (!gameScene->loadMapFromFile("map.dat")) {
        QMessageBox::warning(this, tr("Помилка"), tr("Не вдалося завантажити збереження."));
        delete gameScene;
        gameScene = nullptr;
        return;
    }

    mapView = new GameView(gameScene, this);

    stackedWidget->addWidget(mapView);
    stackedWidget->setCurrentWidget(mapView);
    mapView->setFocus();

    MainHero* hero = gameScene->getHero();
    skillTreeWidget = new SkillTreeWidget(hero, this);
    skillTreeWidget->hide();

    setupPauseWidget();

    btnTree = new QPushButton(this);
    btnTree->setIcon(QIcon(":/textures/icon.png"));
    btnTree->setGeometry(10, height() - 150, 50, 50);
    btnTree->show();
    btnTree->raise();

    connect(btnTree, &QPushButton::clicked, [this]() {
        if (gameScene) gameScene->setPaused(true);
        if (skillTreeWidget) skillTreeWidget->show();
    });

    connect(skillTreeWidget, &SkillTreeWidget::closed, [this]() {
        if (gameScene) gameScene->setPaused(false);
    });

    campfireWidget = new CampfireWidget(this);
    campfireWidget->hide();

    npcWidget = new NPCWidget(this);
    npcWidget->hide();

    connect(gameScene, &GameScene::npcInteractionRequested, this, &MainWindow::onNPCInteractionRequested);
    connect(gameScene, &GameScene::combatRequested, this, &MainWindow::onCombatRequested);
    connect(gameScene, &GameScene::campfireRequested, this, &MainWindow::onCampfireRequested);
    connect(gameScene, &GameScene::combatStarted, this, &MainWindow::onCombatStarted);
    connect(gameScene, &GameScene::combatEnded, this, &MainWindow::onCombatEnded);
    connect(gameScene, &GameScene::gameOver, this, &MainWindow::handleGameOver);
    connect(gameScene, &GameScene::victory, this, &MainWindow::handleVictory);
    connect(gameScene, &GameScene::levelUpTriggered, this, &MainWindow::handleLevelUp);

    heroWidget = new HeroWidget(TextureManager::getInstance().getUnitTexture(UnitType::MainHero, false), gameScene, this);
    heroWidget->setFixedSize(200, 100);
    heroWidget->move(10, height() - heroWidget->height() - 10);
    heroWidget->raise();
    heroWidget->show();

    connect(gameScene, &GameScene::heroStatsChanged, heroWidget, &HeroWidget::updateStats);
    heroWidget->updateStats();
}

void MainWindow::setupPauseWidget()
{
    pauseWidget = new PauseWidget(this);
    pauseWidget->hide();

    pauseWidget->resize(this->size());

    connect(pauseWidget, &PauseWidget::continueClicked, this, &MainWindow::onPauseContinue);
    connect(pauseWidget, &PauseWidget::exitClicked, this, &MainWindow::onPauseExit);
}

void MainWindow::cleanupGame()
{
    if (mapView) {
        stackedWidget->removeWidget(mapView);
        mapView->deleteLater();
        mapView = nullptr;
    }
    if (gameScene) {
        gameScene->deleteLater();
        gameScene = nullptr;
    }
    if (heroWidget) {
        heroWidget->deleteLater();
        heroWidget = nullptr;
    }
    if (skillTreeWidget) {
        skillTreeWidget->deleteLater();
        skillTreeWidget = nullptr;
    }
    if (btnTree) {
        btnTree->deleteLater();
        btnTree = nullptr;
    }
    if (pauseWidget) {
        pauseWidget->deleteLater();
        pauseWidget = nullptr;
    }
    if (fightWidget) {
        fightWidget->deleteLater();
        fightWidget = nullptr;
    }
    if (campfireWidget) {
        campfireWidget->deleteLater();
        campfireWidget = nullptr;
    }
    if (npcWidget) {
        npcWidget->deleteLater();
        npcWidget = nullptr;
    }
}

void MainWindow::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Escape)
    {
        onBtnPauseClicked();
    }
    else
    {
        QMainWindow::keyPressEvent(event);
    }
}

//кнопка паузи
void MainWindow::onBtnPauseClicked()
{
    if (!gameScene) return;

    gameScene->setPaused(true);
    pauseWidget->resize(this->size());
    pauseWidget->raise();
    pauseWidget->show();
}

void MainWindow::onPauseContinue()
{
    if (pauseWidget) pauseWidget->hide();
    if (gameScene) gameScene->setPaused(false);
    if (mapView) mapView->setFocus();
}

void MainWindow::onPauseExit()
{
    if (gameScene) {
        gameScene->saveMapToFile("map.dat");
    }
    cleanupGame();
    stackedWidget->setCurrentWidget(menuWidget);
}

//подія програшу
void MainWindow::handleGameOver()
{
    showEndGameDialog(tr("Гру завершено"), tr("На жаль, ваш герой загинув..."), false);
}

//подія перемоги
void MainWindow::handleVictory()
{
    showEndGameDialog(tr("ПЕРЕМОГА!"), tr("Вітаємо! Ви успішно пройшли гру!"), true);
}

void MainWindow::handleLevelUp()
{
    if (!gameScene) return;

    gameScene->setPaused(true);

    std::vector<UpgradeOption> options = LevelUpGenerator::generateOptions();

    levelUpWidget->showOptions(options);

    int x = (this->width() - levelUpWidget->width()) / 2;
    int y = (this->height() - levelUpWidget->height()) / 2;
    levelUpWidget->move(x, y);

    levelUpWidget->raise();
    levelUpWidget->show();
}

void MainWindow::onLevelUpOptionSelected(int index)
{
    if (!gameScene) return;

    MainHero* hero = gameScene->getHero();

    UpgradeOption option = levelUpWidget->getOption(index);
    hero->applyUpgrade(option);

    hero->decrementLevelUpPending();

    if (hero->isLevelUpPending()) {
        std::vector<UpgradeOption> newOptions = LevelUpGenerator::generateOptions();
        levelUpWidget->showOptions(newOptions);

        return;
    }

    gameScene->setPaused(false);
    if (mapView) {
        mapView->setFocus();
    }
}

void MainWindow::onCombatStarted()
{
    if (heroWidget) heroWidget->hide();
    if (btnTree) btnTree->hide();

    if (skillTreeWidget) skillTreeWidget->hide();
}

void MainWindow::onCombatEnded()
{
    if (heroWidget) heroWidget->show();
    if (btnTree) btnTree->show();
}

void MainWindow::onCombatRequested(Unit* enemy)
{
    QPixmap heroTex = TextureManager::getInstance().getUnitTexture(UnitType::MainHero);
    QPixmap enemyTex = TextureManager::getInstance().getUnitTexture(enemy->getType());

    MainHero* hero = gameScene->getHero();

    fightWidget = new Fight(heroTex, enemyTex, hero, enemy, this);
    fightWidget->resize(this->size());
    fightWidget->show();
    fightWidget->setFocus();

    connect(fightWidget, &Fight::battleEnded, [this, enemy](bool playerWon) {

        bool escaped = fightWidget->didPlayerEscaped();

        if (gameScene) {
            gameScene->finishCombat(playerWon, escaped, enemy);
        }

        fightWidget->deleteLater();
        fightWidget = nullptr;
    });
}

void MainWindow::onCampfireRequested(double oldHP, double newHP, double oldMana, double newMana, int charges, Unit* campfireUnit)
{
    if (!campfireWidget) return;

    campfireWidget->showRestDetails(oldHP, newHP, oldMana, newMana, charges);
    campfireWidget->raise();

    campfireWidget->disconnect(SIGNAL(finished()));

    connect(campfireWidget, &CampfireWidget::finished, [this, campfireUnit]() {
        if (gameScene) {
            gameScene->finishCampfireInteraction(campfireUnit);
        }
    });
}

void MainWindow::onNPCInteractionRequested(Unit* npcUnit, const QString& text)
{
    if (!npcWidget) return;

    QString name = "Friendly Villager";

    npcWidget->showDialogue(name, text);

    npcWidget->disconnect(SIGNAL(finished()));

    connect(npcWidget, &NPCWidget::finished, [this]() {
        if (gameScene) {
            gameScene->finishNPCInteraction();
        }
    });
}

void MainWindow::showEndGameDialog(const QString& title, const QString& message, bool isVictory)
{
    if (gameScene) {
        gameScene->setPaused(true);
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
        cleanupGame();
        stackedWidget->setCurrentWidget(menuWidget);
    });

    frameLayout->addWidget(lblTitle);
    frameLayout->addWidget(lblMsg);
    frameLayout->addWidget(btnMenu, 0, Qt::AlignCenter);

    layout->addWidget(dialogFrame);
    overlay->show();
    overlay->raise();
}
