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
    //каст спелів
    void playerSpellClicked(QListWidgetItem *item);

    //кнопка втечі
    void onEscapeButtonClicked();

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

    // Чий зараз хід
    bool isPlayerTurn;

    //перевірка втечі
    bool playerEscaped = false;

    // Оновлює відображення HP та MP
    void updateStatsDisplay();

    // Заповнює список заклинань гравця
    void populatePlayerSpellList();

    // Додає повідомлення в лог бою
    void appendToCombatLog(const QString& message);

    // Визначає, хто ходить першим
    void determineFirstTurn();

    // Обробляє хід гравця
    void executePlayerTurn(const Spell& spell);

    // Обробляє хід ШІ
    void executeAiTurn();

    // Перевіряє, чи не закінчився бій
    bool checkForEndOfBattle();

     // Завершує бій
    void endBattle(bool playerWon);

    void displayInitialHealth();
    void displayInitialMana();
};

#endif // FIGHT_H
