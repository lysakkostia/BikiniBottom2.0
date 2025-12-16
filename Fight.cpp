#include "Fight.h"
#include "ui_Fight.h"
#include "RandomGenerator.h"
#include "Unit.h"
#include "AI.h"
#include "GameConstants.h"
#include <QResizeEvent>
#include <QTimer>
#include <QMessageBox>
#include <QDebug>

Fight::Fight(const QPixmap& enemyTexture, MainHero* hero, Unit* enemy, QWidget *parent)
    : QDialog(parent)
    , ui(new Ui::Fight)
    , currentEnemyTexture(enemyTexture)
    , fightingHero(hero)
    , currentEnemy(enemy)
    , playerEscaped(false)
{
    ui->setupUi(this);

    initUI();
    setupConnections();
    startBattle();
}

Fight::~Fight()
{
    delete ui;
}

void Fight::initUI()
{
    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setFixedSize(1088, 614);
    setWindowTitle(tr("Fight!"));

    this->setStyleSheet("Fight QMessageBox { background-image: none; border-image: none; }");

    ui->enemyImageLabel->setAlignment(Qt::AlignCenter);
    if (!currentEnemyTexture.isNull()) {
        ui->enemyImageLabel->setPixmap(currentEnemyTexture);
    } else {
        QPixmap placeholder(200, 150);
        placeholder.fill(Qt::gray);
        ui->enemyImageLabel->setPixmap(placeholder);
        qWarning() << "Fight: Enemy texture is null.";
    }

    updateStatsDisplay();
}

void Fight::setupConnections()
{
    connect(ui->spellListWidget, &QListWidget::itemClicked, this, &Fight::onPlayerSpellClicked);
    connect(ui->btn_escape, &QPushButton::clicked, this, &Fight::onEscapeButtonClicked);
}

void Fight::startBattle()
{
    determineFirstTurn();

    if (isPlayerTurn) {
        logMessage(tr("Хід Гравця. Оберіть заклинання:"));
        populatePlayerSpellList();
        ui->spellListWidget->setEnabled(true);
        ui->btn_escape->setEnabled(true);
    } else {
        logMessage(tr("Хід Ворога."));
        ui->spellListWidget->setEnabled(false);
        ui->btn_escape->setEnabled(false);
        prepareAiTurn();
    }
}

void Fight::updateStatsDisplay()
{
    if (!fightingHero || !currentEnemy) return;

    ui->Hero_HP->setText(tr("HP Героя: %1").arg(fightingHero->GetHP()));
    ui->Enemy_HP->setText(tr("HP Ворога: %1").arg(currentEnemy->GetHP()));
    ui->Hero_Mana->setText(tr("MP Героя: %1").arg(fightingHero->GetMana()));
    ui->Enemy_Mana->setText(tr("MP Ворога: %1").arg(currentEnemy->GetMana()));
}

void Fight::populatePlayerSpellList()
{
    if (!fightingHero || !fightingHero->GetAI()) return;

    ui->spellListWidget->clear();
    const std::vector<Spell>& spells = fightingHero->GetAI()->GetSpells();

    for (size_t i = 0; i < spells.size(); ++i) {
        const Spell& spell = spells[i];
        QString spellText = QString("%1 (Мана: %2, Шкода: %3)")
                                .arg(QString::fromStdString(spell.name))
                                .arg(spell.manacost)
                                .arg(spell.damage);

        QListWidgetItem* item = new QListWidgetItem(spellText, ui->spellListWidget);
        item->setData(Qt::UserRole, static_cast<int>(i));
    }
}

void Fight::logMessage(const QString& message)
{
    ui->combatLogTextEdit->append(message);
}

void Fight::logCombatAction(const QString& actorName, const QString& actionName, int damage, int manaCost)
{
    QString msg = tr("%1 використав '%2', завдавши %3 шкоди. Витрачено %4 мани.")
                      .arg(actorName)
                      .arg(actionName)
                      .arg(damage)
                      .arg(manaCost);
    logMessage(msg);
}

void Fight::determineFirstTurn()
{
    if (!fightingHero || !currentEnemy || !fightingHero->GetAI() || !currentEnemy->GetAI()) {
        isPlayerTurn = true;
        return;
    }

    int heroRoll = RandGenerator::RandIntInInterval(0, 20);
    int enemyRoll = RandGenerator::RandIntInInterval(0, 20);

    int heroScore = fightingHero->GetAI()->TurnOver + heroRoll + (fightingHero->GetLevel() * 2);
    int enemyScore = currentEnemy->GetAI()->TurnOver + enemyRoll + (currentEnemy->GetLevel() * 2);

    isPlayerTurn = (heroScore >= enemyScore);

    qDebug() << "Initiative:" << (isPlayerTurn ? "Hero" : "Enemy")
             << "Hero:" << heroScore << "Enemy:" << enemyScore;
}

void Fight::onPlayerSpellClicked(QListWidgetItem *item)
{
    if (!isPlayerTurn || !item) return;

    bool ok;
    int index = item->data(Qt::UserRole).toInt(&ok);
    if (!ok) return;

    AI* heroAI = fightingHero->GetAI();
    const auto& spells = heroAI->GetSpells();

    if (index >= 0 && index < static_cast<int>(spells.size())) {
        const Spell& spell = spells[index];

        if (fightingHero->CanUseMana(spell.manacost)) {
            executePlayerTurn(spell);
        } else {
            logMessage(tr("Герой: недостатньо мани для '%1'!").arg(QString::fromStdString(spell.name)));
        }
    }
}

void Fight::executePlayerTurn(const Spell& spell)
{
    fightingHero->ConsumeMana(spell.manacost);
    currentEnemy->TakeDamage(spell.damage);

    logCombatAction(tr("Гравець"), QString::fromStdString(spell.name), spell.damage, spell.manacost);
    updateStatsDisplay();

    if (isBattleOver()) return;

    isPlayerTurn = false;
    ui->spellListWidget->setEnabled(false);
    ui->btn_escape->setEnabled(false);

    logMessage(tr("Хід Ворога."));

    QTimer::singleShot(GlobalConst::FightAI::AI_AFTER_PLAYER_DELAY_MS + RandGenerator::RandIntInInterval(0, 500), this, &Fight::onAiTurnTimeout);
}

void Fight::prepareAiTurn()
{
    QTimer::singleShot(GlobalConst::FightAI::AI_THINK_DELAY_MS, this, &Fight::onAiTurnTimeout);
}

void Fight::onAiTurnTimeout()
{
    executeAiTurn();
}

void Fight::executeAiTurn()
{
    if (isPlayerTurn || isBattleOver()) return;

    logMessage(tr("Ворог розмірковує..."));

    AI* enemyAI = currentEnemy->GetAI();
    if (!enemyAI) {
        logMessage(tr("Ворог розгублений (AI not found)."));
        isPlayerTurn = true;
        populatePlayerSpellList();
        ui->spellListWidget->setEnabled(true);
        ui->btn_escape->setEnabled(true);
        return;
    }

    const Spell* chosenSpell = enemyAI->ChooseBestSpell(currentEnemy->GetMana());

    if (chosenSpell && currentEnemy->CanUseMana(chosenSpell->manacost)) {
        currentEnemy->ConsumeMana(chosenSpell->manacost);
        fightingHero->TakeDamage(chosenSpell->damage);

        logCombatAction(tr("Ворог"), QString::fromStdString(chosenSpell->name), chosenSpell->damage, chosenSpell->manacost);
    } else {
        logMessage(tr("Ворог пропускає хід (недостатньо мани або заклинань)."));
    }

    updateStatsDisplay();

    if (isBattleOver()) return;

    isPlayerTurn = true;
    logMessage(tr("Хід Гравця. Оберіть дію:"));
    populatePlayerSpellList();
    ui->spellListWidget->setEnabled(true);
    ui->btn_escape->setEnabled(true);
}

bool Fight::isBattleOver()
{
    if (fightingHero->GetHP() <= 0) {
        logMessage(tr("Герой переможений!"));
        endBattle(false);
        return true;
    }
    if (currentEnemy->GetHP() <= 0) {
        logMessage(tr("Ворог переможений!"));
        endBattle(true);
        return true;
    }
    return false;
}

void Fight::endBattle(bool playerWon)
{
    ui->spellListWidget->setEnabled(false);
    ui->btn_escape->setEnabled(false);

    if (playerWon) {
        QMessageBox::information(this, tr("Перемога!"), tr("Ви перемогли ворога!"));
        accept();
    } else {
        QMessageBox::critical(this, tr("Поразка"), tr("Вас було переможено..."));
        reject();
    }
}

void Fight::onEscapeButtonClicked()
{
    if (!isPlayerTurn) return;

    logMessage(tr("Гравець намагається втекти..."));
    ui->spellListWidget->setEnabled(false);
    ui->btn_escape->setEnabled(false);

    bool success = (RandGenerator::RandDoubleInInterval(0.0, 1.0) <= 0.5);

    if (success) {
        logMessage(tr("Втеча вдалася!"));

        currentEnemy->SetHP(currentEnemy->GetMaxHP());
        currentEnemy->SetMana(currentEnemy->GetMaxMana());

        QMessageBox::information(this, tr("Втеча"), tr("Ви успішно втекли!"));
        playerEscaped = true;
        reject();
    } else {
        logMessage(tr("Втекти не вдалося! Хід втрачено."));
        QMessageBox::warning(this, tr("Невдача"), tr("Втекти не вдалося!"));

        isPlayerTurn = false;
        logMessage(tr("Хід Ворога."));
        QTimer::singleShot(GlobalConst::FightAI::AI_AFTER_PLAYER_DELAY_MS, this, &Fight::onAiTurnTimeout);
    }
}

bool Fight::didPlayerEscaped() const
{
    return playerEscaped;
}

void Fight::resizeEvent(QResizeEvent* event)
{
    QDialog::resizeEvent(event);
    if (ui->enemyImageLabel && !currentEnemyTexture.isNull()) {
        ui->enemyImageLabel->setPixmap(currentEnemyTexture.scaled(ui->enemyImageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
}
