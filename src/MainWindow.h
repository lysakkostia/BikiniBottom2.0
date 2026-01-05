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
#include "TutorialWidget.h"

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void onBtnPlayClicked();
    void onBtnSettingsClicked();
    void onBtnExitClicked();
    void onBtnPauseClicked();

    void handleBackToMenu();

    void startNewGame(const QString& worldName, const QString& seed, int radius);
    void loadSavedGame(const QString& filePath);
    void handleGameOver();
    void handleVictory();

    void handleVolumeChanged(int volume);

    void handleLevelUp();
    void onLevelUpOptionSelected(int index);

    void onPauseContinue();
    void onPauseExit();

    void onCombatStarted();
    void onCombatEnded();
    void onCombatRequested(Unit* enemy);
    void onCampfireRequested(double oldHP, double newHP, double oldMana, double newMana, int charges, Unit* campfireUnit);
    void onNPCInteractionRequested(Unit* npcUnit, const QString& text);

private:
    QWidget* menuWidget = nullptr;
    QPushButton* btnPlay = nullptr;
    QPushButton* btnSettings = nullptr;
    QPushButton* btnExit = nullptr;
    QPushButton* btnTree = nullptr;
    QLabel* lblTitle = nullptr;
    QLabel* lblFooter = nullptr;
    QPushButton* btnTutorial = nullptr;

    QStackedWidget* stackedWidget = nullptr;

    SettingsWidget* settingsWidget = nullptr;
    GameSelectionWidget* gameSelectWidget = nullptr;
    GameView* mapView = nullptr;
    GameScene* gameScene = nullptr;
    HeroWidget* heroWidget = nullptr;
    SkillTreeWidget* skillTreeWidget = nullptr;
    PauseWidget* pauseWidget = nullptr;
    LevelUpWidget* levelUpWidget = nullptr;
    Fight* fightWidget = nullptr;
    CampfireWidget* campfireWidget = nullptr;
    NPCWidget* npcWidget = nullptr;
    TutorialWidget* tutorialWidget = nullptr;

    QMediaPlayer* mediaPlayer;
    QAudioOutput* audioOutput;

    void cleanupGame();
    void setupGameSession();
    void setupPauseWidget();
    void initializeMenuUI();
    void showEndGameDialog(const QString& title, const QString& message, bool isVictory);
    void setupTutorialWidget();
};
#endif // MAINWINDOW_H
