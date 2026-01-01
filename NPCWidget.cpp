#include "NPCWidget.h"
#include <QPainter>
#include <QResizeEvent>

NPCWidget::NPCWidget(QWidget *parent) : QWidget(parent) {
    this->setWindowFlags(Qt::FramelessWindowHint);

    QVBoxLayout* overlayLayout = new QVBoxLayout(this);
    overlayLayout->setAlignment(Qt::AlignCenter);

    container = new QWidget(this);
    container->setFixedSize(800, 300);
    container->setStyleSheet(
        "QWidget {"
        "    background-color: #1a1a1a;"
        "    border: 3px solid #D4AF37;"
        "    border-radius: 10px;"
        "}"
        );

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setColor(Qt::black);
    shadow->setOffset(0, 5);
    container->setGraphicsEffect(shadow);

    overlayLayout->addWidget(container);

    QVBoxLayout* contentLayout = new QVBoxLayout(container);
    contentLayout->setContentsMargins(25, 25, 25, 25);

    nameLabel = new QLabel("NPC Name", container);
    nameLabel->setStyleSheet("border: none; font-size: 24px; font-weight: bold; color: #FFD700; margin-bottom: 10px;");
    contentLayout->addWidget(nameLabel);

    textLabel = new QLabel("Hello traveler...", container);
    textLabel->setStyleSheet("border: none; font-size: 18px; color: #ffffff;");
    textLabel->setWordWrap(true);
    textLabel->setAlignment(Qt::AlignTop | Qt::AlignLeft);
    contentLayout->addWidget(textLabel, 1);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->addStretch();

    closeBtn = new QPushButton("Goodbye", container);
    closeBtn->setCursor(Qt::PointingHandCursor);
    closeBtn->setFixedSize(150, 40);
    closeBtn->setStyleSheet(
        "QPushButton {"
        "    background-color: #333;"
        "    color: white;"
        "    border: 1px solid #555;"
        "    border-radius: 5px;"
        "    font-size: 16px;"
        "}"
        "QPushButton:hover {"
        "    background-color: #444;"
        "    border: 1px solid #D4AF37;"
        "}"
        );

    connect(closeBtn, &QPushButton::clicked, [this]() {
        this->hide();
        emit finished();
    });

    btnLayout->addWidget(closeBtn);
    contentLayout->addLayout(btnLayout);
}

void NPCWidget::showDialogue(const QString& unitName, const QString& dialogueText) {
    nameLabel->setText(unitName);
    textLabel->setText(dialogueText);

    if (parentWidget()) {
        this->resize(parentWidget()->size());
    }

    this->raise();
    this->show();
}

void NPCWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(this->rect(), QColor(0, 0, 0, 100));
}
