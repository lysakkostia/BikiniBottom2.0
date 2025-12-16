#ifndef FIGHT_H
#define FIGHT_H

#include <QDialog>
#include <QPixmap>
#include <QListWidget>

class Unit;
class MainHero;
class Spell;

namespace Ui {
class Fight;
}

class Fight : public QDialog
{
    Q_OBJECT

private slots:
    void onPlayerSpellClicked(QListWidgetItem *item);
    void onEscapeButtonClicked();
    void onAiTurnTimeout();

public:

    explicit Fight(const QPixmap& enemyTexture, MainHero* hero, Unit* enemy, QWidget *parent = nullptr);
    ~Fight();

    bool didPlayerEscaped() const;

protected:
    void resizeEvent(QResizeEvent* event) override;

private:
    Ui::Fight *ui;
    QPixmap currentEnemyTexture;
    MainHero* fightingHero;
    Unit* currentEnemy;
    bool isPlayerTurn;
    bool playerEscaped = false;

    void initUI();
    void setupConnections();
    void startBattle();

    void updateStatsDisplay();
    void populatePlayerSpellList();
    void logMessage(const QString& message);
    void logCombatAction(const QString& actorName, const QString& actionName, int damage, int manaCost);

    void determineFirstTurn();
    void executePlayerTurn(const Spell& spell);
    void prepareAiTurn();
    void executeAiTurn();

    bool isBattleOver();
    void endBattle(bool playerWon);
};

#endif // FIGHT_H
