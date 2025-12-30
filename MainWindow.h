#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QStackedWidget>
#include <QMediaPlayer>
#include <QAudioOutput>
#include <QPushButton>
#include <QLabel>
#include <QWidget>

#include "SettingsWidget.h"
#include "GameSelectionWidget.h"
#include "GameView.h"
#include "GameScene.h"
#include "PauseWidget.h"
#include "HeroWidget.h"
#include "LevelUpWidget.h"
#include "SkillTreeWidget.h"
#include "Fight.h"
#include "CampfireWidget.h"
#include "NPCWidget.h"
#include "TextureManager.h"

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
    void OnCombatRequested(Unit* enemy);
    void OnCampfireRequested(double oldHP, double newHP, double oldMana, double newMana, int charges, Unit* campfireUnit);
    void OnNPCInteractionRequested(Unit* npcUnit, const QString& text);

private:
    QWidget *m_menuWidget;
    QPushButton *btn_play;
    QPushButton *btn_settings;
    QPushButton *btn_exit;
    QLabel *lbl_title;
    QLabel *lbl_footer;

    QStackedWidget *m_stackedWidget;

    SettingsWidget *m_settingsWidget;
    GameSelectionWidget *m_gameSelectWidget;

    GameView* MapView = nullptr;
    GameScene* MGameScene = nullptr;
    HeroWidget* heroWidget = nullptr;
    SkillTreeWidget* skillTreeWidget = nullptr;
    QPushButton* m_treeBtn = nullptr;
    PauseWidget* m_pauseWidget = nullptr;
    LevelUpWidget* levelUpWidget = nullptr;
    Fight* m_fightWidget = nullptr;
    CampfireWidget* m_campfireWidget = nullptr;
    NPCWidget* m_npcWidget = nullptr;

    QMediaPlayer *player;
    QAudioOutput *audioOutput;

    int MapRadius = 10;

    void CleanupGame();
    void SetupPauseWidget();
    void InitializeMenuUI();
    void showEndGameDialog(const QString& title, const QString& message, bool isVictory);
};
#endif // MAINWINDOW_H
