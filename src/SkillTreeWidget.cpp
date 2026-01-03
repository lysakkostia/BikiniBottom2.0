#include "SkillTreeWidget.h"
#include "Unit.h"

#include <QLabel>
#include <QPainter>
#include <QMouseEvent>
#include <QResizeEvent>
#include <QShowEvent>
#include <QKeyEvent>
#include <QFrame>
#include <QVBoxLayout>
#include <cmath>
#include <QDebug>

SkillNodeButton::SkillNodeButton(const SkillNode& data, QWidget* parent)
    : QPushButton(parent), nodeId(data.id), nodeData(data) {
    this->setFixedSize(60, 60);
    this->setCursor(Qt::PointingHandCursor);
}

SkillTreeWidget::SkillTreeWidget(MainHero* h, QWidget* parent)
    : QWidget(parent),
    hero(h),
    selectedBtn(nullptr),
    scaleFactor(1.0),
    panOffset(0, 0),
    isDragging(false)
{

    this->setWindowFlags(Qt::FramelessWindowHint);
    this->setMouseTracking(true);

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
    centerBtn->setIcon(QIcon(":/textures/icon.png"));
    centerBtn->setIconSize(QSize(60, 60));

    setupInfoPanel();
    initializeTree();
}

void SkillTreeWidget::setupInfoPanel() {
    infoPanel = new QFrame(this);
    infoPanel->setFixedWidth(300);
    infoPanel->setStyleSheet("background-color: rgba(30, 30, 30, 230); border-left: 2px solid #555; border-radius: 10px;");

    QVBoxLayout* layout = new QVBoxLayout(infoPanel);
    layout->setContentsMargins(20, 40, 20, 20);
    layout->setSpacing(15);

    infoTitleLabel = new QLabel("Select a Skill", infoPanel);
    infoTitleLabel->setStyleSheet("color: #FFD700; font-size: 22px; font-weight: bold; background: transparent; border: none;");
    infoTitleLabel->setWordWrap(true);
    infoTitleLabel->setAlignment(Qt::AlignCenter);
    layout->addWidget(infoTitleLabel);

    infoDescLabel = new QLabel("Click on any node to see details.", infoPanel);
    infoDescLabel->setStyleSheet("color: #DDD; font-size: 16px; background: transparent; border: none;");
    infoDescLabel->setWordWrap(true);
    infoDescLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    layout->addWidget(infoDescLabel);

    infoCostLabel = new QLabel("", infoPanel);
    infoCostLabel->setStyleSheet("color: #AAA; font-size: 14px; font-style: italic; background: transparent; border: none;");
    infoCostLabel->setAlignment(Qt::AlignRight);
    layout->addWidget(infoCostLabel);

    layout->addStretch();

    infoUnlockBtn = new QPushButton("Unlock", infoPanel);
    infoUnlockBtn->setCursor(Qt::PointingHandCursor);
    infoUnlockBtn->setFixedHeight(50);
    infoUnlockBtn->setStyleSheet(
        "QPushButton { background-color: #4CAF50; color: white; font-size: 18px; font-weight: bold; border-radius: 8px; }"
        "QPushButton:hover { background-color: #45a049; }"
        "QPushButton:disabled { background-color: #555; color: #888; }"
        );
    connect(infoUnlockBtn, &QPushButton::clicked, this, &SkillTreeWidget::onUnlockBtnClicked);
    layout->addWidget(infoUnlockBtn);

    infoPanel->hide();
}

void SkillTreeWidget::initializeTree() {
    auto nodes = SkillTreeData::getAllNodes();

    for (const auto& node : nodes) {
        SkillNodeButton* btn = new SkillNodeButton(node, this);

        double angleDeg = getAngleForType(node.branchType);
        double angleRad = angleDeg * M_PI / 180.0;
        int dist = node.tier * RADIUS_STEP;

        btn->logicalPosition = QPointF(dist * std::cos(angleRad), dist * std::sin(angleRad));

        connect(btn, &QPushButton::clicked, [this, btn]() {
            onNodeClicked(btn);
        });
        buttons.push_back(btn);
    }
    updateUI();
}

void SkillTreeWidget::updateNodePositions() {
    int cx = width() / 2;
    int cy = height() / 2;

    QPoint centerScreenPos = QPoint(cx, cy) + panOffset;
    centerBtn->move(centerScreenPos.x() - centerBtn->width()/2, centerScreenPos.y() - centerBtn->height()/2);

    for (auto btn : buttons) {
        QPointF scaledPos = btn->logicalPosition * scaleFactor;
        QPoint finalPos = centerScreenPos + scaledPos.toPoint();

        btn->move(finalPos.x() - btn->width()/2, finalPos.y() - btn->height()/2);
    }
}

void SkillTreeWidget::onNodeClicked(SkillNodeButton* btn) {
    selectedBtn = btn;
    updateInfoPanel();
    infoPanel->show();
    infoPanel->raise();
    updateUI();
}

void SkillTreeWidget::updateInfoPanel() {
    if (!selectedBtn) return;

    const SkillNode& data = selectedBtn->nodeData;
    infoTitleLabel->setText(data.name);

    QString typeStr;
    if (data.effect == SkillEffect::UnlockSpell) typeStr = "Effect: Unlocks Spell";
    else if (data.effect == SkillEffect::IncreaseDamage) typeStr = QString("Effect: Damage +%1%").arg(data.value * 100);
    else if (data.effect == SkillEffect::ReduceManaCost) typeStr = QString("Effect: Mana Cost -%1%").arg(data.value * 100);

    infoDescLabel->setText(QString("%1\n\n%2").arg(data.description, typeStr));

    bool isUnlocked = hero->isNodeUnlocked(data.id);
    bool parentUnlocked = (data.parentId == "root") ? true : hero->isNodeUnlocked(data.parentId);
    bool canAfford = hero->getSkillPoints() >= data.cost;

    if (isUnlocked) {
        infoCostLabel->setText("Status: Unlocked");
        infoUnlockBtn->setText("Purchased");
        infoUnlockBtn->setEnabled(false);
        infoUnlockBtn->setStyleSheet("background-color: #555; color: #888; border-radius: 8px;");
    } else {
        infoCostLabel->setText(QString("Cost: %1 SP").arg(data.cost));
        if (parentUnlocked) {
            if (canAfford) {
                infoUnlockBtn->setText("Unlock");
                infoUnlockBtn->setEnabled(true);
                infoUnlockBtn->setStyleSheet("QPushButton { background-color: #4CAF50; color: white; font-weight: bold; border-radius: 8px; } QPushButton:hover { background-color: #45a049; }");
            } else {
                infoUnlockBtn->setText("Not Enough SP");
                infoUnlockBtn->setEnabled(false);
                infoUnlockBtn->setStyleSheet("background-color: #8B0000; color: white; border-radius: 8px;");
            }
        } else {
            infoUnlockBtn->setText("Locked (Path)");
            infoUnlockBtn->setEnabled(false);
            infoUnlockBtn->setStyleSheet("background-color: #333; color: #666; border-radius: 8px;");
        }
    }
}

void SkillTreeWidget::onUnlockBtnClicked() {
    if (!selectedBtn) return;

    SkillNode& data = selectedBtn->nodeData;

    if (hero->isNodeUnlocked(data.id)) return;
    if (hero->getSkillPoints() < data.cost) return;

    if (hero->unlockSkillNode(data.id, data.cost)) {
        if (data.effect == SkillEffect::UnlockSpell) {
            hero->unlockSpell(data.spellId);
        }
        else if (data.effect == SkillEffect::IncreaseDamage) {
            hero->addSpellDamageMultiplier(data.branchType, data.value);
        }
        else if (data.effect == SkillEffect::ReduceManaCost) {
            hero->addManaCostReduction(data.branchType, data.value);
        }

        updateUI();
        updateInfoPanel();
    }
}

void SkillTreeWidget::updateUI() {
    pointsLabel->setText(QString("Skill Points: %1").arg(hero->getSkillPoints()));
    pointsLabel->adjustSize();

    for (auto btn : buttons) {
        bool unlocked = hero->isNodeUnlocked(btn->nodeId);
        bool parentUnlocked = (btn->nodeData.parentId == "root") ? true : hero->isNodeUnlocked(btn->nodeData.parentId);
        bool canBuy = parentUnlocked && !unlocked && (hero->getSkillPoints() >= btn->nodeData.cost);
        bool isSelected = (btn == selectedBtn);

        QColor typeColor = getColorForType(btn->nodeData.branchType);
        QString colorStyle = typeColor.name();

        QString borderStyle;
        if (isSelected) {
            borderStyle = "border: 4px solid #FFFFFF;";
        } else if (unlocked) {
            borderStyle = "border: 2px solid white;";
        } else if (canBuy) {
            borderStyle = QString("border: 3px solid %1;").arg(colorStyle);
        } else {
            borderStyle = "border: 1px solid #444;";
        }

        if (unlocked) {
            btn->setStyleSheet(QString(
                                   "background-color: %1; %2 border-radius: 30px; color: black; font-weight: bold;"
                                   ).arg(colorStyle, borderStyle));
        }
        else if (canBuy) {
            btn->setStyleSheet(QString(
                                   "background-color: #333; %1 border-radius: 30px; color: white;"
                                   ).arg(borderStyle));
        }
        else {
            btn->setStyleSheet(QString("background-color: #1a1a1a; %1 border-radius: 30px; color: #555;"
                                       ).arg(borderStyle));
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

QColor SkillTreeWidget::getColorForType(SpellType type) {
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

double SkillTreeWidget::getAngleForType(SpellType type) {
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

void SkillTreeWidget::wheelEvent(QWheelEvent *event) {
    double angle = event->angleDelta().y();
    double factor = (angle > 0) ? 1.1 : 0.9;

    double newScale = scaleFactor * factor;

    if (newScale < 0.3) newScale = 0.3;
    if (newScale > 2.0) newScale = 2.0;

    QPoint screenCenter(width()/2, height()/2);
    QPoint mousePos = event->position().toPoint();
    QPoint mouseFromCenter = mousePos - screenCenter - panOffset;

    QPointF oldWorldPos = QPointF(mousePos - screenCenter - panOffset) / scaleFactor;
    scaleFactor = newScale;
    QPointF newPanF = mousePos - screenCenter - (oldWorldPos * scaleFactor);
    panOffset = newPanF.toPoint();

    updateNodePositions();
    updateUI();
}

void SkillTreeWidget::mousePressEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        isDragging = true;
        lastMousePos = event->pos();
        setCursor(Qt::ClosedHandCursor);
    }
    else if (event->button() == Qt::LeftButton) {
        selectedBtn = nullptr;
        infoPanel->hide();
        updateUI();
     }
}

void SkillTreeWidget::mouseMoveEvent(QMouseEvent *event) {
    if (isDragging) {
        QPoint delta = event->pos() - lastMousePos;
        panOffset += delta;
        lastMousePos = event->pos();
        updateNodePositions();
        update();
    }
}

void SkillTreeWidget::mouseReleaseEvent(QMouseEvent *event) {
    if (event->button() == Qt::RightButton) {
        isDragging = false;
        setCursor(Qt::ArrowCursor);
    }
}

void SkillTreeWidget::showEvent(QShowEvent *event) {
    panOffset = QPoint(0,0);
    scaleFactor = 1.0;
    updateUI();
    updateNodePositions();
    QWidget::showEvent(event);
}

void SkillTreeWidget::resizeEvent(QResizeEvent* event) {
    if (parentWidget()) {
        this->setGeometry(0, 0, parentWidget()->width(), parentWidget()->height());
    }

    pointsLabel->move(30, 30);
    closeBtn->setGeometry(width() - 60, 20, 40, 40);

    infoPanel->setGeometry(width() - 320, 80, 300, height() - 160);

    updateNodePositions();
    QWidget::resizeEvent(event);
}



void SkillTreeWidget::paintEvent(QPaintEvent* event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.setRenderHint(QPainter::Antialiasing);

    painter.fillRect(this->rect(), QColor(20, 20, 25, 245));

    QPoint rootPos = centerBtn->geometry().center();

    for (auto btn : buttons) {
        QPoint startPos;
        bool parentFound = false;

        if (btn->nodeData.parentId == "root") {
            startPos = rootPos;
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
            QColor lineColor = getColorForType(btn->nodeData.branchType);

            if (hero->isNodeUnlocked(btn->nodeId)) {
                QPen pen(lineColor, 4 * scaleFactor);
                painter.setPen(pen);
            } else if (hero->isNodeUnlocked(btn->nodeData.parentId) || btn->nodeData.parentId == "root") {
                QPen pen(lineColor.darker(150), 2 * scaleFactor, Qt::DashLine);
                painter.setPen(pen);
            } else {
                QPen pen(QColor("#333"), 2 * scaleFactor);
                painter.setPen(pen);
            }
            painter.drawLine(startPos, endPos);
        }
    }
}

void SkillTreeWidget::keyPressEvent(QKeyEvent* event) {
    if(event->key() == Qt::Key_Escape) closeTree();
}
