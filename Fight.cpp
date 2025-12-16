#include "Fight.h"
#include "ui_Fight.h"
#include "RandomGenerator.h"
#include <QLabel>
#include <QResizeEvent>
#include <QVBoxLayout>
#include "Unit.h"
#include <QDebug>
#include <QTimer>
#include <QMessageBox>
#include <QListWidget>

Fight::Fight(const QPixmap& enemyTexture, MainHero* hero, Unit* enemy, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Fight)
    , currentEnemyTexture(enemyTexture)
    , fightingHero(hero)
    , currentEnemy(enemy)
    , playerEscaped(false)
{

//стилі
    this->setAttribute(Qt::WA_StyledBackground, true);
    ui->setupUi(this);
    this->setFixedSize( 1088, 614 );
    setWindowTitle(tr("Fight!"));
    this->setStyleSheet(
        "Fight QMessageBox {"
        "    background-image: none;"
        "    border-image: none;"
        "}"
        );

// Відображення текстури ворога
    if (ui->enemyImageLabel) {
        ui->enemyImageLabel->setAlignment(Qt::AlignCenter);
        if (!currentEnemyTexture.isNull()) {
            ui->enemyImageLabel->setPixmap(currentEnemyTexture);
        } else {
            QPixmap placeholder(200, 150);

            placeholder.fill(Qt::gray);
            ui->enemyImageLabel->setPixmap(placeholder);
            qWarning("Fight: Enemy texture is null, showing placeholder.");
        }
    } else {
        qWarning("Fight.cpp: ui->enemyImageLabel is null. Please define it in Fight.ui.");
        QLabel* manualLabel = new QLabel(this);
        manualLabel->setAlignment(Qt::AlignCenter);
        if (!currentEnemyTexture.isNull()) {
            manualLabel->setPixmap(currentEnemyTexture);
        }

        if (layout()) {
            layout()->addWidget(manualLabel);
        } else {
            QVBoxLayout* fallbackLayout = new QVBoxLayout(this);
            this->setLayout(fallbackLayout);
            fallbackLayout->addWidget(manualLabel);
        }
    }

    //список спелів
    if (ui->spellListWidget) {
        connect(ui->spellListWidget, &QListWidget::itemClicked, this, &Fight::playerSpellClicked);
    } else {
        qWarning("Fight.cpp: ui->spellListWidget is null! Please define it in Fight.ui.");
    }


    //кнопка втечі
    if (ui->btn_escape) {
        connect(ui->btn_escape, &QPushButton::clicked, this, &Fight::onEscapeButtonClicked);
    } else {
        qWarning("Fight.cpp: ui->escapeButton is null! Please define it in Fight.ui.");
    }

    determineFirstTurn();

    if (ui->spellListWidget) {
        populatePlayerSpellList();
        ui->spellListWidget->setEnabled(isPlayerTurn);
    }
    if (ui->btn_escape) {
        ui->btn_escape->setEnabled(isPlayerTurn);
    }

    // Логіка старту бою
    if (isPlayerTurn) {
        appendToCombatLog(tr("Хід Гравця. Оберіть заклинання:"));
    } else {
        appendToCombatLog(tr("Хід Ворога."));
        // Даємо ворогу час на подумати
        QTimer::singleShot(1500, this, &Fight::executeAiTurn);
    }

    // Відображення статів
    displayInitialHealth();
    displayInitialMana();
}

Fight::~Fight()
{
    delete ui; //
}

//Перевірка на успішну втечу
bool Fight::didPlayerEscaped() const { return playerEscaped; }

//Скейл картинки
void Fight::resizeEvent(QResizeEvent* event)
{
    QDialog::resizeEvent(event); //
    if (ui->enemyImageLabel && !currentEnemyTexture.isNull()) { //
        int w = ui->enemyImageLabel->width();
        int h = ui->enemyImageLabel->height();
        if (w > 0 && h > 0) {
            ui->enemyImageLabel->setPixmap(currentEnemyTexture.scaled(w, h)); //
        }
    }
}

//метод для відображення HP
void Fight::displayInitialHealth()
{
    if (!fightingHero || !currentEnemy) {
        qWarning("Fight::displayInitialHealth: Hero or Enemy unit is null.");
        return;
    }

    if (ui->Hero_HP) {
        ui->Hero_HP->setText(tr("HP Героя: %1").arg(fightingHero->GetHP()));
    } else {
        qWarning("Fight.cpp: ui->heroHpLabel (або ваше ім'я для HP героя) is null. Please define it in Fight.ui.");
    }

    if (ui->Enemy_HP) {
        ui->Enemy_HP->setText(tr("HP Ворога: %1").arg(currentEnemy->GetHP()));
    } else {
        qWarning("Fight.cpp: ui->>Enemy_HP is null. Please define it in Fight.ui.");
    }
}

//метод для відображення Mana
void Fight::displayInitialMana()
{
    if (!fightingHero || !currentEnemy) {
        qWarning("Fight::displayInitialMana: Hero or Enemy unit is null.");
        return;
    }

    if (ui->Hero_Mana) {
        ui->Hero_Mana->setText(tr("MP Героя: %1").arg(fightingHero->GetMana()));
    } else {
        qWarning("Fight.cpp: ui->Hero_Mana is null. "
                 "Будь ласка, визначте QLabel з objectName 'Hero_Mana' в Fight.ui.");
    }

    if (ui->Enemy_Mana) {
        ui->Enemy_Mana->setText(tr("MP Ворога: %1").arg(currentEnemy->GetMana()));
    } else {
        qWarning("Fight.cpp: ui->Enemy_Mana is null. "
                 "Будь ласка, визначте QLabel з objectName 'Enemy_Mana' в Fight.ui.");
    }
}


void Fight::updateStatsDisplay() {
    displayInitialHealth(); // Оновлює HP
    displayInitialMana();   // Оновлює MP
}

//заповнення списку
void Fight::populatePlayerSpellList() {
    if (!ui->spellListWidget || !fightingHero) {
        qWarning("Не вдалося заповнити список заклинань: відсутній spellListWidget або герой/АІ героя.");
        return;
    }

    AI* heroAI = fightingHero->GetAI();
    if (!heroAI) {
        qWarning("Hero has no AI module!");
        return;
    }

    ui->spellListWidget->clear();
    const std::vector<Spell>& spells = heroAI->GetSpells();

    for (size_t i = 0; i < spells.size(); ++i) {
        const Spell& spell = spells[i];
        // Додаємо перевірку на нульовий spell.damage, щоб уникнути ділення на нуль або нескінченності
        QString spellText = QString::fromStdString(spell.name) +
                            tr(" (Мана: %1, Шкода: %2)")
                                .arg(spell.manacost)
                                .arg(spell.damage);
        QListWidgetItem* item = new QListWidgetItem(spellText, ui->spellListWidget);
        item->setData(Qt::UserRole, QVariant::fromValue(static_cast<int>(i)));
    }
}

//текст в лозі
void Fight::appendToCombatLog(const QString& message) {
    if (ui->combatLogTextEdit) {
        ui->combatLogTextEdit->append(message);
    } else {
        qDebug() << "[LOG_БИТВИ]" << message;
    }
}

//хто перший
void Fight::determineFirstTurn() {
    if (!fightingHero || !currentEnemy) {
        isPlayerTurn = true;
        return;
    }

    AI* heroAI = fightingHero->GetAI();
    AI* enemyAI = currentEnemy->GetAI();

    if (!heroAI || !enemyAI) {
        isPlayerTurn = true;
        return;
    }

    int diceRollHero = RandGenerator::RandIntInInterval(0, 20);
    int diceRollEnemy = RandGenerator::RandIntInInterval(0, 20);

    int levelBonusHero = fightingHero->GetLevel() * 2;
    int levelBonusEnemy = currentEnemy->GetLevel() * 2;

    int heroScore = heroAI->TurnOver + diceRollHero + levelBonusHero;
    int enemyScore = enemyAI->TurnOver + diceRollEnemy + levelBonusEnemy;

    isPlayerTurn = (heroScore >= enemyScore);

    qDebug() << "Initiative Check:";
    qDebug() << "Hero:" << heroScore << "(Base:" << heroAI->TurnOver << "+ Roll:" << diceRollHero << "+ LvlBonus:" << levelBonusHero << ")";
    qDebug() << "Enemy:" << enemyScore << "(Base:" << enemyAI->TurnOver << "+ Roll:" << diceRollEnemy << "+ LvlBonus:" << levelBonusEnemy << ")";

    if (isPlayerTurn) {
        qDebug() << "Result: Player goes first!";
    } else {
        qDebug() << "Result: Enemy goes first!";
    }
}

//каст спелу
void Fight::playerSpellClicked(QListWidgetItem *item) {
    if (!isPlayerTurn || !item || !fightingHero || !currentEnemy) return;

    // Перевірка на смерть
    if (fightingHero->GetHP() <= 0 || currentEnemy->GetHP() <= 0) return;

    AI* heroAI = fightingHero->GetAI();
    if (!heroAI) return;

    bool ok;
    int spellIndex = item->data(Qt::UserRole).toInt(&ok);
    const std::vector<Spell>& heroSpells = heroAI->GetSpells();

    if (!ok || spellIndex < 0 || static_cast<size_t>(spellIndex) >= heroSpells.size()) {
        return;
    }

    const Spell& selectedSpell = heroSpells.at(spellIndex);

    if (fightingHero->CanUseMana(selectedSpell.manacost)) {
        if (ui->spellListWidget) ui->spellListWidget->setEnabled(false);
        executePlayerTurn(selectedSpell);
    } else {
        appendToCombatLog(tr("Герой: недостатньо мани для '%1'!").arg(QString::fromStdString(selectedSpell.name)));
    }
}

//хід гравця
void Fight::executePlayerTurn(const Spell& spell) {

if (!fightingHero || !currentEnemy || !isPlayerTurn) return;

    fightingHero->ConsumeMana(spell.manacost);
    currentEnemy->TakeDamage(spell.damage);


    appendToCombatLog(tr("Гравець використав '%1', завдавши %2 шкоди. Витрачено %3 мани.")
                          .arg(QString::fromStdString(spell.name))
                          .arg(spell.damage)
                          .arg(spell.manacost));

    updateStatsDisplay();

    if (checkForEndOfBattle()) {
        return; // Бій закінчено
    }

    isPlayerTurn = false;
    if (ui->spellListWidget) ui->spellListWidget->setEnabled(false);
    if (ui->btn_escape) ui->btn_escape->setEnabled(false); // Вимикаємо кнопку втечі
    appendToCombatLog(tr("Хід Ворога."));
    QTimer::singleShot(1000 + RandGenerator::RandIntInInterval(0,1000), this, &Fight::executeAiTurn);
}

//хід ШІ
void Fight::executeAiTurn() {
    if (isPlayerTurn || !currentEnemy || !fightingHero || checkForEndOfBattle()) return;

    AI* enemyAI = currentEnemy->GetAI();
    if (!enemyAI) {
        appendToCombatLog(tr("Ворог не діє."));
        isPlayerTurn = true;
        return;
    }

    appendToCombatLog(tr("Ворог розмірковує..."));

    // Отримуємо поточну ману ворога
    double enemyCurrentMana = currentEnemy->GetMana();
    const Spell* chosenSpell = enemyAI->ChooseBestSpell(enemyCurrentMana);

    if (chosenSpell) {

        if (currentEnemy->CanUseMana(chosenSpell->manacost)) {
            currentEnemy->ConsumeMana(chosenSpell->manacost);
            fightingHero->TakeDamage(chosenSpell->damage);
            appendToCombatLog(tr("Ворог використав '%1', завдавши %2 шкоди. Витрачено %3 мани.")
                                  .arg(QString::fromStdString(chosenSpell->name))
                                  .arg(chosenSpell->damage)
                                  .arg(chosenSpell->manacost));
        } else {

            appendToCombatLog(tr("Ворог: помилка вибору заклинання (не вистачило мани після вибору). Пропускає хід."));
        }
    } else {
        appendToCombatLog(tr("Ворог: не може застосувати жодне заклинання (немає мани або заклинань). Пропускає хід."));
    }

    updateStatsDisplay();

    if (checkForEndOfBattle()) {
        return;
    }

    isPlayerTurn = true;
    appendToCombatLog(tr("Хід Гравця. Оберіть заклинання або спробуйте втекти:"));
    if (ui->spellListWidget) {
        populatePlayerSpellList();
        ui->spellListWidget->setEnabled(true);
    }
    if (ui->btn_escape) ui->btn_escape->setEnabled(true);
}

//перевірка на кінець битви
bool Fight::checkForEndOfBattle() {
    if (fightingHero && fightingHero->GetHP() <= 0) {
        appendToCombatLog(tr("Герой переможений!"));
        endBattle(false); // Гравець програв
        return true;
    } else if (currentEnemy && currentEnemy->GetHP() <= 0) {
        appendToCombatLog(tr("Ворог переможений!"));
        endBattle(true); // Гравець виграв
        return true;
    }
    return false;
}

//кінець битви
void Fight::endBattle(bool playerWon) {
    if (ui->spellListWidget) ui->spellListWidget->setEnabled(false); // Блокуємо подальші дії

    if (playerWon) {
        QMessageBox::information(this, tr("Перемога!"), tr("Ви перемогли ворога!"));
        accept();
    } else {

        reject();
    }
}

//втеча
void Fight::onEscapeButtonClicked()
{
    if (!isPlayerTurn) {
        QMessageBox::information(this, tr("Хід"), tr("Зараз не ваш хід!"));
        return;
    }

    if (!fightingHero || fightingHero->GetHP() <= 0 || !currentEnemy || currentEnemy->GetHP() <= 0) {
        return;
    }

    appendToCombatLog(tr("Гравець намагається втекти..."));
    if (ui->spellListWidget) ui->spellListWidget->setEnabled(false);
    if (ui->btn_escape) ui->btn_escape->setEnabled(false);

    bool escapeSuccessful = (RandGenerator::RandDoubleInInterval(0.0, 1.0) <= 0.5);

    if (escapeSuccessful) {
        appendToCombatLog(tr("Втеча вдалася!"));
        qWarning() << "Спроба втечі: УСПІШНА";

        if(currentEnemy)
        {
            currentEnemy->SetHP(currentEnemy->GetMaxHP());
            currentEnemy->SetMana(currentEnemy->GetMaxMana());
            appendToCombatLog(tr("Ворог відновив сили після втечі!"));
        }

        QMessageBox::information(this, tr("Втеча"), tr("Втеча вдалася!"));
        playerEscaped = true;

        reject();
    } else {
        appendToCombatLog(tr("Втекти не вдалося! Гравець пропускає хід."));
        qWarning() << "Спроба втечі: НЕВДАЛА";


        QMessageBox::warning(this, tr("Втеча"), tr("Втекти не вдалося!"));

        isPlayerTurn = false;
        appendToCombatLog(tr("Хід Ворога."));
        QTimer::singleShot(1000 + RandGenerator::RandIntInInterval(0, 1000), this, &Fight::executeAiTurn);
    }
}
