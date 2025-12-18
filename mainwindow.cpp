#include "mainwindow.h"
#include "./ui_mainwindow.h"
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QUrl>
#include <QPainter>
#include <QKeyEvent>
#include <QMessageBox>
#include "new_or_old_game.h"

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow),
    settingsWindow(nullptr),
    MapRadius(10),
    heroWidget(nullptr)
{
    ui->setupUi(this);

    //стилі
    this->setFixedSize( 1280, 720 );
    QIcon mainWindowIcon("icon.png");
    this->setWindowIcon(mainWindowIcon);
    this->setStyleSheet(
        "QMainWindow {"
        "    background-image: url(background.png);"
        "    background-repeat: no-repeat;"
        "    background-position: center;"
        "    background-attachment: fixed;"
        "    border-image: url(background.png) 0 0 0 0 stretch stretch;"
        "}"
        );

    player = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);
    player->setAudioOutput(audioOutput);
    player->setSource(QUrl::fromLocalFile("adventure.mp3"));
    player->setLoops(QMediaPlayer::Infinite);
    audioOutput->setVolume(0);
    player->play();

}

MainWindow::~MainWindow()
{
    delete ui;
    delete settingsWindow;
    delete heroWidget;
    delete player;
    delete audioOutput;
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
    if(!settingsWindow)
    {
        settingsWindow = new SettingsWindow();
        connect(settingsWindow, &SettingsWindow::MapRadChanged,
                this, &MainWindow::HandleMapRadiusChanged);
        connect(settingsWindow, &SettingsWindow::VolumeChanged, this, &MainWindow::HandleVolumeChanged);
    }

    settingsWindow->SetCurrentRadius(MapRadius);
    settingsWindow->show();
    settingsWindow->activateWindow();
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
    New_or_old_Game *chooseDialog = new New_or_old_Game(this);

    connect(chooseDialog, &New_or_old_Game::startNewGame, this, [=]() {
        if (MapView) {
            MapView->deleteLater();
            MapView = nullptr;
        }
        if (MGameScene) {
            MGameScene->deleteLater();
            MGameScene = nullptr;
        }
        if (heroWidget) {
            heroWidget->hide();
            heroWidget->deleteLater();
            heroWidget = nullptr;
        }

        MGameScene = new GameScene(MapRadius, this);
        MapView = new GameView(MGameScene, this);

        connect(MGameScene, &GameScene::gameOver, this, &MainWindow::HandleGameOver);
        connect(MGameScene, &GameScene::victory, this, &MainWindow::HandleVictory);

        if (!MenuWidget) {
            MenuWidget = takeCentralWidget();
            if (!MenuWidget) {
                qCritical("MainWindow::startNewGame - error saving MenuWidget.");
                if(MapView) { MapView->deleteLater(); MapView = nullptr; }
                QApplication::quit();
                return;
            }
        }

        MenuWidget->hide();
        setCentralWidget(MapView);
        MapView->setFocus();

        QPixmap HeroTexture("NPC5Texture.png");
        if (!HeroTexture.isNull()) {
            heroWidget = new HeroWidget(HeroTexture, MGameScene, this);

            heroWidget->setFixedSize(200, 100);
            int x = 10;
            int y = height() - heroWidget->height() - 10;
            heroWidget->move(x, y);
            heroWidget->raise();
            heroWidget->show();

            connect(MGameScene, &GameScene::heroStatsChanged,
                    heroWidget, &HeroWidget::Update_stats);
        } else {
            qWarning("Failed to load HeroTexture for HeroWidget.");
        }
    });

    connect(chooseDialog, &New_or_old_Game::loadGame, this, [=]() {
        if(MapView) {
            if (centralWidget() == MapView) takeCentralWidget();
            MapView->deleteLater(); MapView = nullptr;
        }
        if (MGameScene) { MGameScene->deleteLater(); MGameScene = nullptr; }
        if (heroWidget) { heroWidget->hide(); heroWidget->deleteLater(); heroWidget = nullptr; }

        MGameScene = new GameScene(MapRadius, this);
        MapView = new GameView(MGameScene, this);

        connect(MGameScene, &GameScene::gameOver, this, &MainWindow::HandleGameOver);
        connect(MGameScene, &GameScene::victory, this, &MainWindow::HandleVictory);

        if (!MGameScene->LoadMapFromFile("map.dat")) {
            QMessageBox::warning(this, tr("Помилка завантаження"), tr("Не вдалося завантажити карту."));

            MapView->deleteLater(); MapView = nullptr;
            MGameScene->deleteLater(); MGameScene = nullptr;

            if (MenuWidget) {
                if (centralWidget() != MenuWidget) setCentralWidget(MenuWidget);
                MenuWidget->show();
            }
            return;
        }

        if (!MenuWidget) {
            MenuWidget = takeCentralWidget();
        }
        if (MenuWidget) MenuWidget->hide();

        setCentralWidget(MapView);
        MapView->setFocus();

        QPixmap HeroTexture("NPC5Texture.png");
        if (!HeroTexture.isNull()) {
            heroWidget = new HeroWidget(HeroTexture, MGameScene, this);
            heroWidget->setFixedSize(200, 100);
            int x = 10;
            int y = height() - heroWidget->height() - 10;
            heroWidget->move(x, y);
            heroWidget->raise();
            heroWidget->show();
            connect(MGameScene, &GameScene::heroStatsChanged,
                    heroWidget, &HeroWidget::Update_stats);
        }
    });

    chooseDialog->exec();
    delete chooseDialog;
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
    if (!MapView || !MapView->isVisible())
    {
        return;
    }

    if (pauseDialog) {
        delete pauseDialog;
    }

    pauseDialog = new Pause(this, MGameScene);

    int result = pauseDialog->exec();
    if (result != QDialog::Accepted)
    {
        QApplication::quit();
    }
}

//подія програшу
void MainWindow::HandleGameOver()
{
    QMessageBox::information(this, tr("Гру завершено"), tr("Ви програли!"));

    if (MenuWidget) {
        setCentralWidget(MenuWidget);
        MenuWidget->show();
    }
    if (MapView) {
        MapView->deleteLater();
        MapView = nullptr;
    }
    if (MGameScene) {
        MGameScene->deleteLater();
        MGameScene = nullptr;
    }
    if (heroWidget) {
        delete heroWidget;
        heroWidget = nullptr;
    }
}

//подія перемоги
void MainWindow::HandleVictory()
{
    QMessageBox::information(this, tr("Перемога!"), tr("Ви виграли гру!"));

    if (MenuWidget) {
        setCentralWidget(MenuWidget);
        MenuWidget->show();
    }

    if (MapView) {
        MapView->deleteLater();
        MapView = nullptr;
    }
    if (MGameScene) {
        MGameScene->deleteLater();
        MGameScene = nullptr;
    }
    if (heroWidget) {
        delete heroWidget;
        heroWidget = nullptr;
    }
}
