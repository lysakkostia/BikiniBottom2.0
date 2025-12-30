#include "SkillTreeWidget.h"
#include "Unit.h"

#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QShowEvent>
#include <QKeyEvent>
#include <cmath>

SkillNodeButton::SkillNodeButton(const SkillNode& data, QWidget* parent)
    : QPushButton(parent), nodeId(data.id), nodeData(data) {
    this->setFixedSize(60, 60);
    this->setCursor(Qt::PointingHandCursor);
}

SkillTreeWidget::SkillTreeWidget(MainHero* h, QWidget* parent)
    : QWidget(parent), hero(h) {

    this->setWindowFlags(Qt::FramelessWindowHint);

    pointsLabel = new QLabel(this);
    pointsLabel->setStyleSheet("font-size: 26px; color: #FFD700; font-weight: bold; background: transparent;");
    pointsLabel->setAlignment(Qt::AlignLeft);

    closeBtn = new QPushButton("X", this);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setStyleSheet(
        "QPushButton { background-color: #8B0000; color: white; font-weight: bold; border-radius: 5px; font-size: 18px; border: 1px solid #ff4444; }"
        "QPushButton:hover { background-color: #FF0000; }"
        );
    connect(closeBtn, &QPushButton::clicked, this, &SkillTreeWidget::closeTree);

    centerBtn = new QPushButton(this);
    centerBtn->setFixedSize(80, 80);
    centerBtn->setStyleSheet("border-radius: 40px; background-color: #2a2a2a; border: 3px solid #FFD700;");
    centerBtn->setIcon(QIcon("icon.png"));
    centerBtn->setIconSize(QSize(60, 60));

    InitializeTree();
}

void SkillTreeWidget::InitializeTree() {
    auto nodes = SkillTreeData::GetAllNodes();

    for (const auto& node : nodes) {
        SkillNodeButton* btn = new SkillNodeButton(node, this);
        connect(btn, &QPushButton::clicked, [this, btn]() {
            OnNodeClicked(btn);
        });
        buttons.push_back(btn);
    }
    UpdateUI();
}

void SkillTreeWidget::OnNodeClicked(SkillNodeButton* btn) {
    if (hero->IsNodeUnlocked(btn->nodeId)) return;

    bool parentUnlocked = false;
    if (btn->nodeData.parentId == "root") {
        parentUnlocked = true;
    } else {
        parentUnlocked = hero->IsNodeUnlocked(btn->nodeData.parentId);
    }

    if (parentUnlocked && hero->GetSkillPoints() >= btn->nodeData.cost) {
        if (hero->UnlockSkillNode(btn->nodeId, btn->nodeData.cost)) {

            if (btn->nodeData.effect == SkillEffect::UnlockSpell) {
                hero->UnlockSpell(btn->nodeData.spellId);
            }
            else if (btn->nodeData.effect == SkillEffect::IncreaseDamage) {
                hero->AddSpellDamageMultiplier(btn->nodeData.branchType, btn->nodeData.value);
            }
            else if (btn->nodeData.effect == SkillEffect::ReduceManaCost) {
                hero->AddManaCostReduction(btn->nodeData.branchType, btn->nodeData.value);
            }

            UpdateUI();
        }
    }
}

void SkillTreeWidget::UpdateUI() {
    pointsLabel->setText(QString("Skill Points: %1").arg(hero->GetSkillPoints()));
    pointsLabel->adjustSize();

    for (auto btn : buttons) {
        bool unlocked = hero->IsNodeUnlocked(btn->nodeId);
        bool parentUnlocked = (btn->nodeData.parentId == "root") ? true : hero->IsNodeUnlocked(btn->nodeData.parentId);
        bool canBuy = parentUnlocked && !unlocked && (hero->GetSkillPoints() >= btn->nodeData.cost);

        QColor typeColor = GetColorForType(btn->nodeData.branchType);
        QString colorStyle = typeColor.name();

        if (unlocked) {
            btn->setStyleSheet(QString(
                                   "background-color: %1; border: 2px solid white; border-radius: 30px; color: black; font-weight: bold;"
                                   ).arg(colorStyle));
            btn->setToolTip(QString("%1 (Unlocked)\n%2").arg(btn->nodeData.name, btn->nodeData.description));
        }
        else if (canBuy) {
            btn->setStyleSheet(QString(
                                   "background-color: #333; border: 3px solid %1; border-radius: 30px; color: white;"
                                   ).arg(colorStyle));
            btn->setToolTip(QString("%1 (Cost: %2)\n%3").arg(btn->nodeData.name).arg(btn->nodeData.cost).arg(btn->nodeData.description));
            btn->setEnabled(true);
        }
        else {
            btn->setStyleSheet("background-color: #1a1a1a; border: 1px solid #444; border-radius: 30px; color: #555;");
            btn->setToolTip("Locked");
        }

        if(btn->nodeData.effect == SkillEffect::UnlockSpell) btn->setText("Spell");
        else if(btn->nodeData.effect == SkillEffect::IncreaseDamage) btn->setText("Dmg");
        else btn->setText("Mana");
    }
    update();
}

void SkillTreeWidget::closeTree() {
    this->hide();
    emit closed();
}

QColor SkillTreeWidget::GetColorForType(SpellType type) {
    switch(type) {
    case SpellType::Fire: return QColor(255, 69, 0);
    case SpellType::Ice: return QColor(0, 191, 255);
    case SpellType::Electric: return QColor(255, 215, 0);
    case SpellType::Poison: return QColor(50, 205, 50);
    case SpellType::Dark: return QColor(138, 43, 226);
    case SpellType::Holy: return QColor(255, 250, 205);
    default: return Qt::gray;
    }
}

double SkillTreeWidget::GetAngleForType(SpellType type) {
    switch(type) {
    case SpellType::Fire: return 0.0;
    case SpellType::Ice: return 60.0;
    case SpellType::Electric: return 120.0;
    case SpellType::Poison: return 180.0;
    case SpellType::Dark: return 240.0;
    case SpellType::Holy: return 300.0;
    default: return 0.0;
    }
}

void SkillTreeWidget::showEvent(QShowEvent *event) {
    UpdateUI();
    QWidget::showEvent(event);
}

void SkillTreeWidget::resizeEvent(QResizeEvent* event) {
    if (parentWidget()) {
        this->setGeometry(0, 0, parentWidget()->width(), parentWidget()->height());
    }

    int cx = width() / 2;
    int cy = height() / 2;

    pointsLabel->move(30, 30);
    closeBtn->setGeometry(width() - 60, 20, 40, 40);
    centerBtn->move(cx - 40, cy - 40);

    for (auto btn : buttons) {
        double angleDeg = GetAngleForType(btn->nodeData.branchType);
        double angleRad = angleDeg * M_PI / 180.0;
        int dist = btn->nodeData.tier * RADIUS_STEP;

        int x = cx + dist * std::cos(angleRad) - 30;
        int y = cy + dist * std::sin(angleRad) - 30;

        btn->move(x, y);
    }

    QWidget::resizeEvent(event);
}

void SkillTreeWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(this->rect(), QColor(20, 20, 25, 240));

    int cx = width() / 2;
    int cy = height() / 2;

    for (auto btn : buttons) {
        QPoint startPos;
        bool parentFound = false;

        if (btn->nodeData.parentId == "root") {
            startPos = QPoint(cx, cy);
            parentFound = true;
        } else {
            for (auto otherBtn : buttons) {
                if (otherBtn->nodeId == btn->nodeData.parentId) {
                    startPos = otherBtn->geometry().center();
                    parentFound = true;
                    break;
                }
            }
        }

        if (parentFound) {
            QPoint endPos = btn->geometry().center();
            QColor lineColor = GetColorForType(btn->nodeData.branchType);

            if (hero->IsNodeUnlocked(btn->nodeId)) {
                painter.setPen(QPen(lineColor, 4));
            } else if (hero->IsNodeUnlocked(btn->nodeData.parentId) || btn->nodeData.parentId == "root") {
                painter.setPen(QPen(lineColor.darker(150), 2, Qt::DashLine));
            } else {
                painter.setPen(QPen(QColor("#333"), 2));
            }
            painter.drawLine(startPos, endPos);
        }
    }
}

void SkillTreeWidget::mousePressEvent(QMouseEvent *event) { event->accept(); }
void SkillTreeWidget::mouseReleaseEvent(QMouseEvent *event) { event->accept(); }
void SkillTreeWidget::mouseMoveEvent(QMouseEvent *event) { event->accept(); }
void SkillTreeWidget::wheelEvent(QWheelEvent *event) { event->accept(); }

void SkillTreeWidget::keyPressEvent(QKeyEvent* event) {
    if(event->key() == Qt::Key_Escape) closeTree();
}
