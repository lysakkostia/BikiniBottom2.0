#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "settingswindow.h"
#include "GameView.h"
#include "GameScene.h"
#include "Pause.h"
#include "herowidget.h"
#include "LevelUpWidget.h"
#include "SkillTreeWidget.h"
#include <QMediaPlayer>
#include <QAudioOutput>
QT_BEGIN_NAMESPACE
namespace Ui {
class HeroWidget;
class MainWindow;
}
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private:
    Ui::MainWindow *ui;
    SettingsWindow *settingsWindow;
    QWidget* MenuWidget=nullptr;
    GameView* MapView = nullptr;
    GameScene* MGameScene = nullptr;
    Pause* pauseDialog=nullptr;
    HeroWidget* heroWidget = nullptr;
    LevelUpWidget* levelUpWidget = nullptr;
    SkillTreeWidget* skillTreeWidget = nullptr;
    QMediaPlayer *player;
    QAudioOutput *audioOutput;
    int MapRadius = 10;

private slots:
    void on_btn_exit_clicked();
    void on_btn_settings_clicked();
    void on_btn_play_clicked();
    void on_btn_pause_clicked();

    void HandleMapRadiusChanged(int NewRadius);
    void HandleGameOver();
    void HandleVictory();
    void HandleVolumeChanged(int volume);

    void HandleLevelUp();
    void OnLevelUpOptionSelected(int index);

protected:
    void keyPressEvent(QKeyEvent *event) override;
};
#endif // MAINWINDOW_H
