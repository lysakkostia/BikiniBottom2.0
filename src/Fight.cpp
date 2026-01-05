#include "Fight.h"
#include "RandomGenerator.h"
#include "Unit.h"
#include "AI.h"
#include "GameConstants.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>
#include <QTimer>
#include <QMessageBox>
#include <QDebug>
#include <QScroller>
#include <QScrollBar>
#include <QScrollArea>

class SpellButton : public QPushButton {
public:
    SpellButton(const QString& name, int cost, int damage, int index, QWidget* parent = nullptr)
        : QPushButton(parent), m_index(index)
    {
        setFixedSize(140, 80);
        setCursor(Qt::PointingHandCursor);

        QVBoxLayout* layout = new QVBoxLayout(this);
        layout->setContentsMargins(5, 5, 5, 5);
        layout->setSpacing(2);

        QLabel* lblName = new QLabel(name, this);
        lblName->setStyleSheet("font-weight: bold; font-size: 14px; color: #E0E0E0;");
        lblName->setAlignment(Qt::AlignCenter);
        lblName->setWordWrap(true);

        QLabel* lblCost = new QLabel(QString("MP: %1").arg(cost), this);
        lblCost->setStyleSheet("color: #64B5F6; font-size: 12px; font-weight: bold;");
        lblCost->setAlignment(Qt::AlignCenter);

        QLabel* lblDmg = new QLabel(QString("DMG: %1").arg(damage), this);
        lblDmg->setStyleSheet("color: #E57373; font-size: 12px; font-weight: bold;");
        lblDmg->setAlignment(Qt::AlignCenter);

        layout->addWidget(lblName);
        layout->addWidget(lblCost);
        layout->addWidget(lblDmg);

        setStyleSheet(
            "SpellButton {"
            "   background-color: rgba(30, 30, 30, 200);"
            "   border: 2px solid #5D4037;"
            "   border-radius: 8px;"
            "}"
            "SpellButton:hover {"
            "   background-color: rgba(50, 50, 50, 230);"
            "   border: 2px solid #FFD54F;"
            "}"
            "SpellButton:pressed {"
            "   background-color: rgba(20, 20, 20, 230);"
            "   margin-top: 2px;"
            "}"
            "SpellButton:disabled {"
            "   background-color: rgba(20, 20, 20, 100);"
            "   border: 2px solid #3E2723;"
            "   color: gray;"
            "}"
        );
    }

    int getIndex() const { return m_index; }

private:
    int m_index;
};

Fight::Fight(const QPixmap& heroTexture, const QPixmap& enemyTexture, MainHero* hero, Unit* enemy, QWidget *parent)
    : QWidget(parent)
    , currentHeroTexture(heroTexture)
    , currentEnemyTexture(enemyTexture)
    , fightingHero(hero)
    , currentEnemy(enemy)
    , playerEscaped(false)
{
    initUI();
    startBattle();
}

Fight::~Fight()
{
}

void Fight::initUI()
{
    if (parentWidget()) {
        this->resize(parentWidget()->size());
    } else {
        this->setFixedSize(1088, 614);
    }

    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setStyleSheet(
        "Fight { "
        "   background-image: url(:/textures/background_main.png);"
        "   background-position: center;"
        "   background-repeat: no-repeat;"
        "   border-image: url(:/textures/background_main.png) 0 0 0 0 stretch stretch;"
        "}"
        "QProgressBar {"
        "   border: 2px solid #3E2723;"
        "   border-radius: 5px;"
        "   text-align: center;"
        "   color: white;"
        "   font-weight: bold;"
        "   background-color: #263238;"
        "}"
        "QProgressBar::chunk[objectName='hp'] { background-color: #D32F2F; }"
        "QProgressBar::chunk[objectName='mana'] { background-color: #1976D2; }"
        );

    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(20, 20, 20, 20);

    turnLabel = new QLabel(tr("Preparation for battle..."), this);
    turnLabel->setAlignment(Qt::AlignCenter);
    turnLabel->setStyleSheet(
        "font-size: 24px; font-weight: bold; color: #FFD54F;"
        "background-color: rgba(30, 30, 30, 200);"
        "border: 2px solid #5D4037;"
        "border-radius: 10px;"
        "padding: 5px 20px;"
        );
    QHBoxLayout* topLabelLayout = new QHBoxLayout();
    topLabelLayout->addStretch();
    topLabelLayout->addWidget(turnLabel);
    topLabelLayout->addStretch();
    mainLayout->addLayout(topLabelLayout);

    QHBoxLayout* arenaLayout = new QHBoxLayout();
    arenaLayout->setAlignment(Qt::AlignCenter);

    QFrame* heroFrame = new QFrame(this);
    heroFrame->setStyleSheet(
        "background-color: rgba(30, 30, 30, 200);"
        "border: 2px solid #5D4037;"
        "border-radius: 15px;"
        );

    QVBoxLayout* heroStatsLayout = new QVBoxLayout(heroFrame);
    heroStatsLayout->setContentsMargins(15, 15, 15, 15);

    heroAvatar = new QLabel(this);
    heroAvatar->setFixedSize(120, 120);
    heroAvatar->setAlignment(Qt::AlignCenter);
    heroAvatar->setStyleSheet(
        "border: 3px solid #FFD54F;"
        "border-radius: 10px;"
        );

    if (!currentHeroTexture.isNull()) {
        heroAvatar->setPixmap(currentHeroTexture.scaled(heroAvatar->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        heroAvatar->setText("HERO");
    }

    heroStatsLayout->addWidget(heroAvatar, 0, Qt::AlignCenter);

    heroStatsLayout->addSpacing(10);

    heroHpBar = new QProgressBar(this);
    heroHpBar->setObjectName("hp");
    heroHpBar->setFormat("HP: %v/%m");
    heroHpBar->setFixedSize(180, 22);

    heroManaBar = new QProgressBar(this);
    heroManaBar->setObjectName("mana");
    heroManaBar->setFormat("MP: %v/%m");
    heroManaBar->setFixedSize(180, 22);

    heroStatsLayout->addWidget(heroHpBar, 0, Qt::AlignCenter);
    heroStatsLayout->addWidget(heroManaBar, 0, Qt::AlignCenter);

    heroStatsLayout->addSpacing(15);

    btnEscape = new QPushButton("ESCAPE", this);
    btnEscape->setCursor(Qt::PointingHandCursor);
    btnEscape->setFixedHeight(35);
    btnEscape->setFixedWidth(180);

    btnEscape->setStyleSheet(
        "QPushButton {"
        "   background-color: rgba(80, 20, 20, 180);"
        "   color: #FFEBEE;"
        "   font-weight: bold;"
        "   border: 1px solid #E57373;"
        "   border-radius: 5px;"
        "   font-size: 13px;"
        "}"
        "QPushButton:hover { background-color: rgba(120, 30, 30, 200); border-color: #FFCDD2; }"
        "QPushButton:pressed { background-color: #B71C1C; }"
        "QPushButton:disabled { background-color: rgba(50, 50, 50, 150); border-color: #555; color: gray; }"
        );
    connect(btnEscape, &QPushButton::clicked, this, &Fight::onEscapeButtonClicked);

    heroStatsLayout->addWidget(btnEscape, 0, Qt::AlignCenter);

    heroStatsLayout->addStretch();

    QFrame* enemyFrame = new QFrame(this);
    enemyFrame->setStyleSheet(
        "background-color: rgba(30, 30, 30, 200);"
        "border: 2px solid #5D4037;"
        "border-radius: 15px;"
        );

    QVBoxLayout* enemyStatsLayout = new QVBoxLayout(enemyFrame);
    enemyStatsLayout->setContentsMargins(15, 15, 15, 15);

    enemyImageLabel = new QLabel(this);
    enemyImageLabel->setFixedSize(120, 120);
    enemyImageLabel->setAlignment(Qt::AlignCenter);
    enemyImageLabel->setStyleSheet(
        "border: 3px solid #EF5350;"
        "border-radius: 10px;"
        );

    if (!currentEnemyTexture.isNull()) {
        enemyImageLabel->setPixmap(currentEnemyTexture.scaled(enemyImageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    enemyStatsLayout->addWidget(enemyImageLabel, 0, Qt::AlignCenter);

    enemyStatsLayout->addSpacing(10);

    enemyHpBar = new QProgressBar(this);
    enemyHpBar->setObjectName("hp");
    enemyHpBar->setFormat("HP: %v/%m");
    enemyHpBar->setFixedSize(180, 22);

    enemyManaBar = new QProgressBar(this);
    enemyManaBar->setObjectName("mana");
    enemyManaBar->setFormat("MP: %v/%m");
    enemyManaBar->setFixedSize(180, 22);

    enemyStatsLayout->addWidget(enemyHpBar, 0, Qt::AlignCenter);
    enemyStatsLayout->addWidget(enemyManaBar, 0, Qt::AlignCenter);
    enemyStatsLayout->addStretch();

    arenaLayout->addWidget(heroFrame, 1);
    arenaLayout->addSpacing(10);
    arenaLayout->addWidget(enemyFrame, 1);

    mainLayout->addLayout(arenaLayout, 3);

    QHBoxLayout* bottomPanel = new QHBoxLayout();
    bottomPanel->setSpacing(20);

    QScrollArea* spellScrollArea = new QScrollArea(this);
    spellScrollArea->setWidgetResizable(true);

    spellScrollArea->setStyleSheet(
        "QScrollArea { background-color: transparent; border: none; }"
        "QScrollBar:vertical {"
        "   border: 1px solid #3E2723;"
        "   background: #263238;"
        "   width: 15px;"
        "   margin: 22px 0 22px 0;"
        "}"
        "QScrollBar::handle:vertical {"
        "   background: #8D6E63;"
        "   min-height: 20px;"
        "   border-radius: 4px;"
        "}"
        "QScrollBar::add-line:vertical, QScrollBar::sub-line:vertical {"
        "   background: #3E2723;"
        "   height: 20px;"
        "   subcontrol-origin: margin;"
        "}"
        );

    QWidget* spellContentWidget = new QWidget(spellScrollArea);
    spellContentWidget->setStyleSheet("background-color: rgba(0, 0, 0, 150); border-radius: 10px;");

    spellsContainer = spellContentWidget;

    spellsGrid = new QGridLayout(spellContentWidget);
    spellsGrid->setSpacing(10);
    spellsGrid->setContentsMargins(10, 10, 10, 10);

    spellsGrid->setRowStretch(999, 1);

    spellScrollArea->setWidget(spellContentWidget);

    bottomPanel->addWidget(spellScrollArea, 1);

    combatLog = new QTextEdit(this);
    combatLog->setReadOnly(true);
    combatLog->setStyleSheet(
        "QTextEdit {"
        "   background-color: rgba(0, 0, 0, 180);"
        "   color: #EEEEEE;"
        "   border: 1px solid #555;"
        "   border-radius: 5px;"
        "   font-family: 'Unispace';"
        "   font-size: 12px;"
        "}"
        );
    bottomPanel->addWidget(combatLog, 1);

    mainLayout->addLayout(bottomPanel, 2);

    updateStatsDisplay();
}

void Fight::startBattle()
{
    determineFirstTurn();

    if (isPlayerTurn) {
        turnLabel->setText(tr("YOUR TURN"));
        turnLabel->setStyleSheet("font-size: 24px; font-weight: bold; background-color: rgba(30, 30, 30, 200); border: 2px solid #5D4037; border-radius: 10px; padding: 5px 20px; color: #66BB6A;");
        logMessage(tr("Player's Turn. Select a spell:"));
        populatePlayerSpellGrid();
        btnEscape->setEnabled(true);
    } else {
        turnLabel->setText(tr("ENEMY'S TURN"));
        turnLabel->setStyleSheet("font-size: 24px; font-weight: bold; background-color: rgba(30, 30, 30, 200); border: 2px solid #5D4037; border-radius: 10px; padding: 5px 20px; color: #EF5350;");
        logMessage(tr("Enemy's Turn."));
        populatePlayerSpellGrid();
        btnEscape->setEnabled(false);
        prepareAiTurn();
    }
}

void Fight::updateStatsDisplay()
{
    if (!fightingHero || !currentEnemy) return;

    heroHpBar->setRange(0, static_cast<int>(fightingHero->getMaxHP()));
    heroHpBar->setValue(static_cast<int>(fightingHero->getHP()));

    heroManaBar->setRange(0, static_cast<int>(fightingHero->getMaxMana()));
    heroManaBar->setValue(static_cast<int>(fightingHero->getMana()));

    enemyHpBar->setRange(0, static_cast<int>(currentEnemy->getMaxHP()));
    enemyHpBar->setValue(static_cast<int>(currentEnemy->getHP()));

    enemyManaBar->setRange(0, static_cast<int>(currentEnemy->getMaxMana()));
    enemyManaBar->setValue(static_cast<int>(currentEnemy->getMana()));
}

void Fight::populatePlayerSpellGrid()
{
    QLayoutItem* item;
    while ((item = spellsGrid->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    if (!fightingHero || !fightingHero->getAI()) return;

    const std::vector<Spell>& spells = fightingHero->getAI()->getSpells();

    int row = 0;
    int col = 0;
    int maxCols = 3;

    for (size_t i = 0; i < spells.size(); ++i) {
        const Spell& spell = spells[i];

        SpellButton* btn = new SpellButton(
            QString::fromStdString(spell.name),
            static_cast<int>(spell.manacost),
            static_cast<int>(spell.damage),
            static_cast<int>(i),
            this
            );

        if (!isPlayerTurn || !fightingHero->canUseMana(spell.manacost)) {
            btn->setEnabled(false);
            if(isPlayerTurn) {
                btn->setToolTip(tr("Not enough mana"));
            }
        } else {
            connect(btn, &QPushButton::clicked, [this, i]() {
                onSpellBtnClicked(static_cast<int>(i));
            });
        }

        spellsGrid->addWidget(btn, row, col);

        col++;
        if (col >= maxCols) {
            col = 0;
            row++;
        }
    }
}

void Fight::onSpellBtnClicked(int spellIndex)
{
    if (!isPlayerTurn) return;

    AI* heroAI = fightingHero->getAI();
    const auto& spells = heroAI->getSpells();

    if (spellIndex >= 0 && spellIndex < static_cast<int>(spells.size())) {
        const Spell& spell = spells[spellIndex];
        if (fightingHero->canUseMana(spell.manacost)) {
            executePlayerTurn(spell);
        }
    }
}

void Fight::logMessage(const QString& message)
{
    combatLog->append(message);
    QScrollBar *sb = combatLog->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void Fight::logCombatAction(const QString& actorName, const QString& actionName, int damage, int manaCost)
{
    QString msg = tr("<span style='color: #FFD54F;'>%1</span> used <b>%2</b>, dealing <span style='color: #EF5350;'>%3 damage</span>. Spent <span style='color: #42A5F5;'>%4 mana</span>.")
                      .arg(actorName)
                      .arg(actionName)
                      .arg(damage)
                      .arg(manaCost);
    combatLog->append(msg);
    QScrollBar *sb = combatLog->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void Fight::determineFirstTurn()
{
    if (!fightingHero || !currentEnemy || !fightingHero->getAI() || !currentEnemy->getAI()) {
        isPlayerTurn = true;
        return;
    }

    int heroRoll = RandGenerator::randIntInInterval(0, 20);
    int enemyRoll = RandGenerator::randIntInInterval(0, 20);

    int heroScore = fightingHero->getAI()->turnOver + heroRoll + (fightingHero->getLevel() * 2);
    int enemyScore = currentEnemy->getAI()->turnOver + enemyRoll + (currentEnemy->getLevel() * 2);

    isPlayerTurn = (heroScore >= enemyScore);

    qDebug() << "Initiative:" << (isPlayerTurn ? "Hero" : "Enemy")
             << "Hero:" << heroScore << "Enemy:" << enemyScore;
}

void Fight::executePlayerTurn(const Spell& spell)
{
    fightingHero->consumeMana(spell.manacost);
    currentEnemy->takeDamage(spell.damage);

    logCombatAction(tr("Player"), QString::fromStdString(spell.name), spell.damage, spell.manacost);
    updateStatsDisplay();

    if (isBattleOver()) return;

    isPlayerTurn = false;
    turnLabel->setText(tr("ENEMY'S TURN"));
    turnLabel->setStyleSheet("font-size: 24px; font-weight: bold; background-color: rgba(30, 30, 30, 200); border: 2px solid #5D4037; border-radius: 10px; padding: 5px 20px; color: #EF5350;");

    populatePlayerSpellGrid();
    btnEscape->setEnabled(false);

    QTimer::singleShot(GlobalConst::FightAI::AI_AFTER_PLAYER_DELAY_MS + RandGenerator::randIntInInterval(0, 500), this, &Fight::onAiTurnTimeout);
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

    logMessage(tr("Enemy is thinking..."));

    AI* enemyAI = currentEnemy->getAI();
    if (!enemyAI) {
        logMessage(tr("Enemy is confused."));
        isPlayerTurn = true;
        return;
    }

    const Spell* chosenSpell = enemyAI->chooseBestSpell(currentEnemy->getMana(), fightingHero->getHP(), currentEnemy->getHP());

    if (chosenSpell && currentEnemy->canUseMana(chosenSpell->manacost)) {
        currentEnemy->consumeMana(chosenSpell->manacost);
        fightingHero->takeDamage(chosenSpell->damage);

        logCombatAction(tr("Enemy"), QString::fromStdString(chosenSpell->name), chosenSpell->damage, chosenSpell->manacost);
    } else {
        logMessage(tr("Enemy skips a turn."));
    }

    updateStatsDisplay();

    if (isBattleOver()) return;

    isPlayerTurn = true;
    turnLabel->setText(tr("YOUR TURN"));
    turnLabel->setStyleSheet("font-size: 24px; font-weight: bold; background-color: rgba(30, 30, 30, 200); border: 2px solid #5D4037; border-radius: 10px; padding: 5px 20px; color: #66BB6A;");

    logMessage(tr("Player's Turn."));
    populatePlayerSpellGrid();
    btnEscape->setEnabled(true);
}

bool Fight::isBattleOver()
{
    if (fightingHero->getHP() <= 0) {
        logMessage(tr("Hero is defeated!"));
        endBattle(false);
        return true;
    }
    if (currentEnemy->getHP() <= 0) {
        logMessage(tr("Enemy is defeated!"));
        endBattle(true);
        return true;
    }
    return false;
}

void Fight::endBattle(bool playerWon)
{
    if(spellsContainer) spellsContainer->setEnabled(false);
    if(btnEscape) btnEscape->setEnabled(false);

    QString title = playerWon ? tr("Victory!") : tr("Defeat...");
    QString message = playerWon ? tr("You have defeated the enemy!") : tr("You have been defeated...");

    showInternalDialog(title, message, [this, playerWon]() {
        emit battleEnded(playerWon);
        this->close();
    });
}

void Fight::onEscapeButtonClicked()
{
    if (!isPlayerTurn) return;

    logMessage(tr("Attempt to escape..."));

    populatePlayerSpellGrid();
    btnEscape->setEnabled(false);

    bool success = (RandGenerator::randDoubleInInterval(0.0, 1.0) <= 0.5);

    if (success) {
        logMessage(tr("Escape was successful!"));
        currentEnemy->setHP(currentEnemy->getMaxHP());
        currentEnemy->setMana(currentEnemy->getMaxMana());
        showInternalDialog(tr("Success!"), tr("You have successfully escaped!"), [this]() {
            playerEscaped = true;
            emit battleEnded(false);
            this->close();
        });
    } else {
        logMessage(tr("Escape failed! You lost your move."));
        showInternalDialog(tr("Failure"), tr("Escape failed!"), [this]() {
            isPlayerTurn = false;
            turnLabel->setText(tr("ENEMY'S TURN"));
            turnLabel->setStyleSheet("font-size: 24px; font-weight: bold; background-color: rgba(30, 30, 30, 200); border: 2px solid #5D4037; border-radius: 10px; padding: 5px 20px; color: #EF5350;");

            QTimer::singleShot(GlobalConst::FightAI::AI_AFTER_PLAYER_DELAY_MS, this, &Fight::onAiTurnTimeout);
        });
    }
}

bool Fight::didPlayerEscaped() const
{
    return playerEscaped;
}

void Fight::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);
    if (currentOverlay) {
        currentOverlay->setGeometry(rect());
    }
}

void Fight::showInternalDialog(const QString& title, const QString& message, std::function<void()> onOk)
{
    if (currentOverlay) {
        currentOverlay->deleteLater();
    }

    currentOverlay = new QWidget(this);
    currentOverlay->setGeometry(rect());
    currentOverlay->setStyleSheet("background-color: rgba(0, 0, 0, 180);");

    QVBoxLayout* overlayLayout = new QVBoxLayout(currentOverlay);
    overlayLayout->setAlignment(Qt::AlignCenter);

    QFrame* dialogPanel = new QFrame(currentOverlay);
    dialogPanel->setFixedSize(380, 220);
    dialogPanel->setStyleSheet(
        "QFrame {"
        "   background-color: #3E2723;"
        "   border: 3px solid #FFD54F;"
        "   border-radius: 15px;"
        "}"
        "QLabel { background: transparent; color: #E0E0E0; font-size: 16px; border: none; }"
        );

    QVBoxLayout* dialogLayout = new QVBoxLayout(dialogPanel);
    dialogLayout->setSpacing(15);
    dialogLayout->setContentsMargins(20, 20, 20, 20);

    QLabel* lblTitle = new QLabel(title, dialogPanel);
    lblTitle->setAlignment(Qt::AlignCenter);
    lblTitle->setStyleSheet("font-size: 22px; font-weight: bold; color: #FFD54F;");

    QLabel* lblMsg = new QLabel(message, dialogPanel);
    lblMsg->setAlignment(Qt::AlignCenter);
    lblMsg->setWordWrap(true);

    QPushButton* btnOk = new QPushButton("OK", dialogPanel);
    btnOk->setCursor(Qt::PointingHandCursor);
    btnOk->setFixedSize(100, 40);
    btnOk->setStyleSheet(
        "QPushButton {"
        "   background-color: #5D4037; color: white; font-weight: bold;"
        "   border: 2px solid #8D6E63; border-radius: 8px;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover { background-color: #6D4C41; border-color: #FFD54F; }"
        "QPushButton:pressed { background-color: #3E2723; }"
        );

    connect(btnOk, &QPushButton::clicked, [this, onOk]() {
        if (currentOverlay) {
            currentOverlay->deleteLater();
            currentOverlay = nullptr;
        }
        if (onOk) {
            onOk();
        }
    });

    dialogLayout->addWidget(lblTitle);
    dialogLayout->addWidget(lblMsg);
    dialogLayout->addWidget(btnOk, 0, Qt::AlignCenter);

    overlayLayout->addWidget(dialogPanel);
    currentOverlay->show();
}
