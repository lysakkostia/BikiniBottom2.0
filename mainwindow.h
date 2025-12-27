#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QMediaPlayer>
#include <QAudioOutput>

#include "SettingsWidget.h"
#include "GameSelectionWidget.h"
#include "GameView.h"
#include "GameScene.h"
#include "PauseWidget.h"
#include "HeroWidget.h"
#include "LevelUpWidget.h"
#include "SkillTreeWidget.h"

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

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void on_btn_play_clicked();
    void on_btn_settings_clicked();
    void on_btn_exit_clicked();
    void on_btn_pause_clicked();

    void HandleBackToMenu();

    void StartNewGame();
    void LoadSavedGame();
    void HandleGameOver();
    void HandleVictory();

    void HandleMapRadiusChanged(int NewRadius);
    void HandleVolumeChanged(int volume);

    void HandleLevelUp();
    void OnLevelUpOptionSelected(int index);

    void OnPauseContinue();
    void OnPauseExit();

    void OnCombatStarted();
    void OnCombatEnded();

private:
    Ui::MainWindow *ui;

    QStackedWidget *m_stackedWidget;
    QWidget *m_menuWidget;
    SettingsWidget *m_settingsWidget;
    GameSelectionWidget *m_gameSelectWidget;

    GameView* MapView = nullptr;
    GameScene* MGameScene = nullptr;
    HeroWidget* heroWidget = nullptr;
    SkillTreeWidget* skillTreeWidget = nullptr;
    QPushButton* m_treeBtn = nullptr;
    PauseWidget* m_pauseWidget = nullptr;
    LevelUpWidget* levelUpWidget = nullptr;

    QMediaPlayer *player;
    QAudioOutput *audioOutput;

    int MapRadius = 10;

    void CleanupGame();
    void SetupPauseWidget();
};
#endif // MAINWINDOW_H
