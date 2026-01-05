#include "CampfireWidget.h"
#include <QPainter>
#include <QResizeEvent>

CampfireWidget::CampfireWidget(QWidget *parent) : QWidget(parent) {
    this->setWindowFlags(Qt::FramelessWindowHint);

    QVBoxLayout* overlayLayout = new QVBoxLayout(this);
    overlayLayout->setAlignment(Qt::AlignCenter);

    QWidget* container = new QWidget(this);
    container->setFixedSize(500, 350);
    container->setStyleSheet(
        "QWidget {"
        "    background-color: #1a1a1a;"
        "    border: 3px solid #FF4500;"
        "    border-radius: 15px;"
        "}"
        );

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setColor(Qt::black);
    shadow->setOffset(0, 5);
    container->setGraphicsEffect(shadow);

    overlayLayout->addWidget(container);

    QVBoxLayout* contentLayout = new QVBoxLayout(container);
    contentLayout->setContentsMargins(30, 30, 30, 30);
    contentLayout->setSpacing(15);

    titleLabel = new QLabel("Campfire Rest", container);
    titleLabel->setStyleSheet("border: none; font-size: 28px; font-weight: bold; color: #FF8C00;");
    titleLabel->setAlignment(Qt::AlignCenter);
    contentLayout->addWidget(titleLabel);

    QFrame* line = new QFrame(container);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);
    line->setStyleSheet("background-color: #555; border: none; max-height: 1px;");
    contentLayout->addWidget(line);

    hpLabel = new QLabel(container);
    hpLabel->setStyleSheet("border: none; font-size: 18px; color: #dddddd;");
    hpLabel->setAlignment(Qt::AlignCenter);
    contentLayout->addWidget(hpLabel);

    manaLabel = new QLabel(container);
    manaLabel->setStyleSheet("border: none; font-size: 18px; color: #dddddd;");
    manaLabel->setAlignment(Qt::AlignCenter);
    contentLayout->addWidget(manaLabel);

    statusLabel = new QLabel(container);
    statusLabel->setStyleSheet("border: none; font-size: 16px; font-style: italic; color: #aaaaaa; margin-top: 10px;");
    statusLabel->setAlignment(Qt::AlignCenter);
    statusLabel->setWordWrap(true);
    contentLayout->addWidget(statusLabel);

    contentLayout->addStretch();

    leaveBtn = new QPushButton("Leave", container);
    leaveBtn->setCursor(Qt::PointingHandCursor);
    leaveBtn->setFixedHeight(45);
    leaveBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #333;"
        "    color: white;"
        "    border: 1px solid #FF4500;"
        "    border-radius: 8px;"
        "    font-size: 16px;"
        "    font-weight: bold;"
        "}"
        "QPushButton:hover {"
        "    background-color: #444;"
        "    border: 2px solid #FFA500;"
        "}"
        );
    contentLayout->addWidget(leaveBtn);

    connect(leaveBtn, &QPushButton::clicked, [this]() {
        this->hide();
        emit finished();
    });
}

void CampfireWidget::showRestDetails(double oldHP, double newHP, double oldMana, double newMana, int remainingCharges) {
    hpLabel->setText(formatStatChange("HP", oldHP, newHP, "#32CD32"));
    manaLabel->setText(formatStatChange("Mana", oldMana, newMana, "#1E90FF"));

    if (remainingCharges > 0) {
        statusLabel->setText(QString("The fire crackles warmly.\nRemaining firewood: %1").arg(remainingCharges));
        titleLabel->setText("Rest by the Fire");
    } else {
        statusLabel->setText("The fire fades into embers and turns to ash.\nThe campfire is extinguished.");
        titleLabel->setText("Fire Extinguished");
    }

    if (parentWidget()) {
        this->resize(parentWidget()->size());
    }

    this->raise();
    this->show();
}

QString CampfireWidget::formatStatChange(QString name, double oldVal, double newVal, QString color) {
    int diff = static_cast<int>(newVal - oldVal);
    QString sign = (diff >= 0) ? "+" : "";

    return QString("%1: %2 -> <span style='color:%5; font-weight:bold;'>%3</span> "
                   "(<span style='color:%5;'>%6%4</span>)")
        .arg(name)
        .arg(static_cast<int>(oldVal))
        .arg(static_cast<int>(newVal))
        .arg(diff)
        .arg(color)
        .arg(sign);
}

void CampfireWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(this->rect(), QColor(0, 0, 0, 150));
}
