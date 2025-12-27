#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QUrl>
#include <QMessageBox>
#include <QDebug>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
    , m_settingsWidget(nullptr)
    , MapRadius(10)
    , heroWidget(nullptr)
{
    ui->setupUi(this);

    this->setFixedSize(1280, 720);
    this->setWindowIcon(QIcon("icon.png"));

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

    m_menuWidget = takeCentralWidget();
    m_stackedWidget->addWidget(m_menuWidget);

    m_settingsWidget = new SettingsWidget(this);
    m_stackedWidget->addWidget(m_settingsWidget);

    m_gameSelectWidget = new GameSelectionWidget(this);
    m_stackedWidget->addWidget(m_gameSelectWidget);

    setCentralWidget(m_stackedWidget);

    connect(m_settingsWidget, &SettingsWidget::MapRadChanged, this, &MainWindow::HandleMapRadiusChanged);
    connect(m_settingsWidget, &SettingsWidget::VolumeChanged, this, &MainWindow::HandleVolumeChanged);
    connect(m_settingsWidget, &SettingsWidget::BackClicked, this, &MainWindow::HandleBackToMenu);

    connect(m_gameSelectWidget, &GameSelectionWidget::StartNewGameClicked, this, &MainWindow::StartNewGame);
    connect(m_gameSelectWidget, &GameSelectionWidget::LoadGameClicked, this, &MainWindow::LoadSavedGame);
    connect(m_gameSelectWidget, &GameSelectionWidget::BackClicked, this, &MainWindow::HandleBackToMenu);

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

    delete ui;
    delete player;
    delete audioOutput;
    delete levelUpWidget;
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
    QMessageBox::information(this, tr("Гру завершено"), tr("Ви програли!"));
    CleanupGame();
    m_stackedWidget->setCurrentWidget(m_menuWidget);
}

//подія перемоги
void MainWindow::HandleVictory()
{
    QMessageBox::information(this, tr("Перемога!"), tr("Ви виграли гру!"));
    CleanupGame();
    m_stackedWidget->setCurrentWidget(m_menuWidget);
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
