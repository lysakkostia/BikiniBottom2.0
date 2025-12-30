#ifndef FIGHT_H
#define FIGHT_H

#include <QWidget>
#include <QPixmap>
#include <QLabel>
#include <QProgressBar>
#include <QTextEdit>
#include <QGridLayout>
#include <QPushButton>
#include <QScrollArea>
#include <functional>

class Unit;
class MainHero;
class Spell;

class SpellButton;

class Fight : public QWidget
{
    Q_OBJECT

public:
    explicit Fight(const QPixmap& heroTexture, const QPixmap& enemyTexture, MainHero* hero, Unit* enemy, QWidget *parent = nullptr);
    ~Fight();

    bool didPlayerEscaped() const;

signals:
    void battleEnded(bool playerWon);

protected:
    void resizeEvent(QResizeEvent* event) override;

private slots:
    void onSpellBtnClicked(int spellIndex);
    void onEscapeButtonClicked();
    void onAiTurnTimeout();

private:
    QPixmap currentHeroTexture;
    QPixmap currentEnemyTexture;
    MainHero* fightingHero;
    Unit* currentEnemy;
    bool isPlayerTurn;
    bool playerEscaped = false;

    QLabel* m_enemyImageLabel;
    QLabel* m_turnLabel;

    QLabel* m_heroAvatar;
    QProgressBar* m_heroHpBar;
    QProgressBar* m_heroManaBar;

    QProgressBar* m_enemyHpBar;
    QProgressBar* m_enemyManaBar;

    QWidget* m_spellsContainer;
    QGridLayout* m_spellsGrid;

    QPushButton* m_btnEscape;
    QTextEdit* m_combatLog;

    QWidget* m_currentOverlay = nullptr;

    void initUI();
    void startBattle();

    void updateStatsDisplay();
    void populatePlayerSpellGrid();

    void logMessage(const QString& message);
    void logCombatAction(const QString& actorName, const QString& actionName, int damage, int manaCost);

    void determineFirstTurn();
    void executePlayerTurn(const Spell& spell);
    void prepareAiTurn();
    void executeAiTurn();

    bool isBattleOver();
    void endBattle(bool playerWon);

    void showInternalDialog(const QString& title, const QString& message, std::function<void()> onOk = nullptr);
};

#endif // FIGHT_H
