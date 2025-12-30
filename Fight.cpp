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
        "   background-image: url(TB2.png);"
        "   background-position: center;"
        "   background-repeat: no-repeat;"
        "   border-image: url(TB2.png) 0 0 0 0 stretch stretch;"
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

    m_turnLabel = new QLabel(tr("Підготовка до бою..."), this);
    m_turnLabel->setAlignment(Qt::AlignCenter);
    m_turnLabel->setStyleSheet("font-size: 24px; font-weight: bold; color: #FFD54F; text-shadow: 2px 2px #000;");
    mainLayout->addWidget(m_turnLabel);

    QHBoxLayout* arenaLayout = new QHBoxLayout();

    QVBoxLayout* heroStatsLayout = new QVBoxLayout();
    m_heroAvatar = new QLabel(this);
    m_heroAvatar->setFixedSize(120, 120);
    m_heroAvatar->setAlignment(Qt::AlignCenter);

    m_heroAvatar->setStyleSheet(
        "border: 3px solid #FFD54F;"
        "border-radius: 10px;"
        "background-color: rgba(0,0,0,100);"
        );

    if (!currentHeroTexture.isNull()) {
        m_heroAvatar->setPixmap(currentHeroTexture.scaled(m_heroAvatar->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    } else {
        m_heroAvatar->setText("HERO");
    }

    heroStatsLayout->addWidget(m_heroAvatar, 0, Qt::AlignCenter);

    m_heroHpBar = new QProgressBar(this);
    m_heroHpBar->setObjectName("hp");
    m_heroHpBar->setFormat("HP: %v/%m");
    m_heroHpBar->setFixedSize(180, 22);

    m_heroManaBar = new QProgressBar(this);
    m_heroManaBar->setObjectName("mana");
    m_heroManaBar->setFormat("MP: %v/%m");
    m_heroManaBar->setFixedSize(180, 22);

    heroStatsLayout->addWidget(m_heroHpBar, 0, Qt::AlignCenter);
    heroStatsLayout->addWidget(m_heroManaBar, 0, Qt::AlignCenter);
    heroStatsLayout->addStretch();

    QVBoxLayout* enemyStatsLayout = new QVBoxLayout();
    m_enemyImageLabel = new QLabel(this);
    m_enemyImageLabel->setFixedSize(120, 120);
    m_enemyImageLabel->setAlignment(Qt::AlignCenter);
    m_enemyImageLabel->setStyleSheet(
        "border: 3px solid #EF5350;"
        "border-radius: 10px;"
        "background-color: rgba(0,0,0,100);"
        );

    if (!currentEnemyTexture.isNull()) {
        m_enemyImageLabel->setPixmap(currentEnemyTexture.scaled(m_enemyImageLabel->size(), Qt::KeepAspectRatio, Qt::SmoothTransformation));
    }
    enemyStatsLayout->addWidget(m_enemyImageLabel, 0, Qt::AlignCenter);

    m_enemyHpBar = new QProgressBar(this);
    m_enemyHpBar->setObjectName("hp");
    m_enemyHpBar->setFormat("HP: %v/%m");
    m_enemyHpBar->setFixedSize(180, 22);

    m_enemyManaBar = new QProgressBar(this);
    m_enemyManaBar->setObjectName("mana");
    m_enemyManaBar->setFormat("MP: %v/%m");
    m_enemyManaBar->setFixedSize(180, 22);

    enemyStatsLayout->addWidget(m_enemyHpBar, 0, Qt::AlignCenter);
    enemyStatsLayout->addWidget(m_enemyManaBar, 0, Qt::AlignCenter);
    enemyStatsLayout->addStretch();

    arenaLayout->addLayout(heroStatsLayout, 1);
    arenaLayout->addSpacing(50);
    arenaLayout->addLayout(enemyStatsLayout, 1);

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

    m_spellsContainer = spellContentWidget;

    m_spellsGrid = new QGridLayout(spellContentWidget);
    m_spellsGrid->setSpacing(10);
    m_spellsGrid->setContentsMargins(10, 10, 10, 10);

    m_spellsGrid->setRowStretch(999, 1);

    spellScrollArea->setWidget(spellContentWidget);

    bottomPanel->addWidget(spellScrollArea, 2);

    QVBoxLayout* centerCtrlLayout = new QVBoxLayout();
    centerCtrlLayout->setAlignment(Qt::AlignCenter);

    m_btnEscape = new QPushButton("ESCAPE", this);
    m_btnEscape->setFixedSize(120, 60);
    m_btnEscape->setCursor(Qt::PointingHandCursor);
    m_btnEscape->setStyleSheet(
        "QPushButton {"
        "   background-color: #5D4037;"
        "   color: white;"
        "   font-weight: bold;"
        "   border: 2px solid #8D6E63;"
        "   border-radius: 10px;"
        "   font-size: 14px;"
        "}"
        "QPushButton:hover { background-color: #6D4C41; border-color: #FFD54F; }"
        "QPushButton:pressed { background-color: #3E2723; }"
        "QPushButton:disabled { background-color: gray; border-color: #444; }"
        );
    connect(m_btnEscape, &QPushButton::clicked, this, &Fight::onEscapeButtonClicked);

    centerCtrlLayout->addWidget(m_btnEscape);
    bottomPanel->addLayout(centerCtrlLayout, 1);

    m_combatLog = new QTextEdit(this);
    m_combatLog->setReadOnly(true);
    m_combatLog->setStyleSheet(
        "QTextEdit {"
        "   background-color: rgba(0, 0, 0, 180);"
        "   color: #EEEEEE;"
        "   border: 1px solid #555;"
        "   border-radius: 5px;"
        "   font-family: 'Unispace';"
        "   font-size: 12px;"
        "}"
        );
    bottomPanel->addWidget(m_combatLog, 2);

    mainLayout->addLayout(bottomPanel, 2);

    updateStatsDisplay();
}

void Fight::startBattle()
{
    determineFirstTurn();

    if (isPlayerTurn) {
        m_turnLabel->setText(tr("ВАШ ХІД"));
        m_turnLabel->setStyleSheet("color: #66BB6A; font-size: 24px; font-weight: bold;");
        logMessage(tr("Хід Гравця. Оберіть заклинання:"));
        populatePlayerSpellGrid();
        m_btnEscape->setEnabled(true);
    } else {
        m_turnLabel->setText(tr("ХІД ВОРОГА"));
        m_turnLabel->setStyleSheet("color: #EF5350; font-size: 24px; font-weight: bold;");
        logMessage(tr("Хід Ворога."));
        populatePlayerSpellGrid();
        m_btnEscape->setEnabled(false);
        prepareAiTurn();
    }
}

void Fight::updateStatsDisplay()
{
    if (!fightingHero || !currentEnemy) return;

    m_heroHpBar->setRange(0, static_cast<int>(fightingHero->GetMaxHP()));
    m_heroHpBar->setValue(static_cast<int>(fightingHero->GetHP()));

    m_heroManaBar->setRange(0, static_cast<int>(fightingHero->GetMaxMana()));
    m_heroManaBar->setValue(static_cast<int>(fightingHero->GetMana()));

    m_enemyHpBar->setRange(0, static_cast<int>(currentEnemy->GetMaxHP()));
    m_enemyHpBar->setValue(static_cast<int>(currentEnemy->GetHP()));

    m_enemyManaBar->setRange(0, static_cast<int>(currentEnemy->GetMaxMana()));
    m_enemyManaBar->setValue(static_cast<int>(currentEnemy->GetMana()));
}

void Fight::populatePlayerSpellGrid()
{
    QLayoutItem* item;
    while ((item = m_spellsGrid->takeAt(0)) != nullptr) {
        delete item->widget();
        delete item;
    }

    if (!fightingHero || !fightingHero->GetAI()) return;

    const std::vector<Spell>& spells = fightingHero->GetAI()->GetSpells();

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

        if (!isPlayerTurn || !fightingHero->CanUseMana(spell.manacost)) {
            btn->setEnabled(false);
            if(isPlayerTurn) {
                btn->setToolTip(tr("Недостатньо мани"));
            }
        } else {
            connect(btn, &QPushButton::clicked, [this, i]() {
                onSpellBtnClicked(static_cast<int>(i));
            });
        }

        m_spellsGrid->addWidget(btn, row, col);

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

    AI* heroAI = fightingHero->GetAI();
    const auto& spells = heroAI->GetSpells();

    if (spellIndex >= 0 && spellIndex < static_cast<int>(spells.size())) {
        const Spell& spell = spells[spellIndex];
        if (fightingHero->CanUseMana(spell.manacost)) {
            executePlayerTurn(spell);
        }
    }
}

void Fight::logMessage(const QString& message)
{
    m_combatLog->append(message);
    QScrollBar *sb = m_combatLog->verticalScrollBar();
    sb->setValue(sb->maximum());
}

void Fight::logCombatAction(const QString& actorName, const QString& actionName, int damage, int manaCost)
{
    QString msg = tr("<span style='color: #FFD54F;'>%1</span> використав <b>%2</b>, завдавши <span style='color: #EF5350;'>%3 шкоди</span>. Витрачено <span style='color: #42A5F5;'>%4 мани</span>.")
                      .arg(actorName)
                      .arg(actionName)
                      .arg(damage)
                      .arg(manaCost);
    m_combatLog->append(msg);
    QScrollBar *sb = m_combatLog->verticalScrollBar();
    sb->setValue(sb->maximum());
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

void Fight::executePlayerTurn(const Spell& spell)
{
    fightingHero->ConsumeMana(spell.manacost);
    currentEnemy->TakeDamage(spell.damage);

    logCombatAction(tr("Гравець"), QString::fromStdString(spell.name), spell.damage, spell.manacost);
    updateStatsDisplay();

    if (isBattleOver()) return;

    isPlayerTurn = false;
    m_turnLabel->setText(tr("ХІД ВОРОГА"));
    m_turnLabel->setStyleSheet("color: #EF5350; font-size: 24px; font-weight: bold;");

    populatePlayerSpellGrid();
    m_btnEscape->setEnabled(false);

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
        logMessage(tr("Ворог розгублений."));
        isPlayerTurn = true;
        return;
    }

    const Spell* chosenSpell = enemyAI->ChooseBestSpell(currentEnemy->GetMana());

    if (chosenSpell && currentEnemy->CanUseMana(chosenSpell->manacost)) {
        currentEnemy->ConsumeMana(chosenSpell->manacost);
        fightingHero->TakeDamage(chosenSpell->damage);

        logCombatAction(tr("Ворог"), QString::fromStdString(chosenSpell->name), chosenSpell->damage, chosenSpell->manacost);
    } else {
        logMessage(tr("Ворог пропускає хід."));
    }

    updateStatsDisplay();

    if (isBattleOver()) return;

    isPlayerTurn = true;
    m_turnLabel->setText(tr("ВАШ ХІД"));
    m_turnLabel->setStyleSheet("color: #66BB6A; font-size: 24px; font-weight: bold;");

    logMessage(tr("Хід Гравця."));
    populatePlayerSpellGrid();
    m_btnEscape->setEnabled(true);
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
    if(m_spellsContainer) m_spellsContainer->setEnabled(false);
    if(m_btnEscape) m_btnEscape->setEnabled(false);

    QString title = playerWon ? tr("Перемога!") : tr("Поразка");
    QString message = playerWon ? tr("Ви перемогли ворога!") : tr("Вас було переможено...");

    showInternalDialog(title, message, [this, playerWon]() {
        emit battleEnded(playerWon);
        this->close();
    });
}

void Fight::onEscapeButtonClicked()
{
    if (!isPlayerTurn) return;

    logMessage(tr("Спроба втечі..."));

    populatePlayerSpellGrid();
    m_btnEscape->setEnabled(false);

    bool success = (RandGenerator::RandDoubleInInterval(0.0, 1.0) <= 0.5);

    if (success) {
        logMessage(tr("Втеча вдалася!"));
        currentEnemy->SetHP(currentEnemy->GetMaxHP());
        currentEnemy->SetMana(currentEnemy->GetMaxMana());
        showInternalDialog(tr("Втеча"), tr("Ви успішно втекли!"), [this]() {
            playerEscaped = true;
            emit battleEnded(false);
            this->close();
        });
    } else {
        logMessage(tr("Втекти не вдалося! Хід втрачено."));
        showInternalDialog(tr("Невдача"), tr("Втекти не вдалося!"), [this]() {
            isPlayerTurn = false;
            m_turnLabel->setText(tr("ХІД ВОРОГА"));
            m_turnLabel->setStyleSheet("color: #EF5350; font-size: 24px; font-weight: bold;");

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
    if (m_currentOverlay) {
        m_currentOverlay->setGeometry(rect());
    }
}

void Fight::showInternalDialog(const QString& title, const QString& message, std::function<void()> onOk)
{
    if (m_currentOverlay) {
        m_currentOverlay->deleteLater();
    }

    m_currentOverlay = new QWidget(this);
    m_currentOverlay->setGeometry(rect());
    m_currentOverlay->setStyleSheet("background-color: rgba(0, 0, 0, 180);");

    QVBoxLayout* overlayLayout = new QVBoxLayout(m_currentOverlay);
    overlayLayout->setAlignment(Qt::AlignCenter);

    QFrame* dialogPanel = new QFrame(m_currentOverlay);
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
        if (m_currentOverlay) {
            m_currentOverlay->deleteLater();
            m_currentOverlay = nullptr;
        }
        if (onOk) {
            onOk();
        }
    });

    dialogLayout->addWidget(lblTitle);
    dialogLayout->addWidget(lblMsg);
    dialogLayout->addWidget(btnOk, 0, Qt::AlignCenter);

    overlayLayout->addWidget(dialogPanel);
    m_currentOverlay->show();
}
