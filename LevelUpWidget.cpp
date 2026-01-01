#include "LevelUpWidget.h"

#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPushButton>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QMouseEvent>
#include <QResizeEvent>

LevelUpWidget::LevelUpWidget(QWidget *parent) : QWidget(parent) {
    this->setWindowFlags(Qt::FramelessWindowHint);

    QVBoxLayout* overlayLayout = new QVBoxLayout(this);
    overlayLayout->setAlignment(Qt::AlignCenter);

    QWidget* container = new QWidget(this);
    container->setFixedSize(900, 550);
    container->setStyleSheet(
        "QWidget {"
        //"    background-image: url(background.png);"
        "    background-color: #1a1a1a;"
        "    border: 3px solid #D4AF37;"
        "    border-radius: 15px;"
        "}"
        );

    QGraphicsDropShadowEffect* winShadow = new QGraphicsDropShadowEffect();
    winShadow->setBlurRadius(20);
    winShadow->setColor(Qt::black);
    winShadow->setOffset(0, 5);
    container->setGraphicsEffect(winShadow);

    overlayLayout->addWidget(container);

    QVBoxLayout* contentLayout = new QVBoxLayout(container);
    contentLayout->setContentsMargins(30, 30, 30, 30);
    contentLayout->setSpacing(20);

    QLabel* title = new QLabel("LEVEL UP!", container);
    title->setStyleSheet("background: transparent; border: none; font-size: 36px; font-weight: bold; color: #FFD700;");
    title->setAlignment(Qt::AlignCenter);
    contentLayout->addWidget(title);

    QLabel* subtitle = new QLabel("Choose your reward:", container);
    subtitle->setStyleSheet("background: transparent; border: none; font-size: 18px; color: #DDDDDD;");
    subtitle->setAlignment(Qt::AlignCenter);
    contentLayout->addWidget(subtitle);

    optionsLayout = new QHBoxLayout();
    optionsLayout->setSpacing(20);
    contentLayout->addLayout(optionsLayout);
}

void LevelUpWidget::showOptions(const std::vector<UpgradeOption>& options) {
    QLayoutItem* item;
    while ((item = optionsLayout->takeAt(0)) != nullptr) {
        if (item->widget()) delete item->widget();
        delete item;
    }
    currentOptions = options;

    for (size_t i = 0; i < options.size(); ++i) {
        QPushButton* btn = new QPushButton(this);

        btn->setStyleSheet(
            "QPushButton {"
            "    background-color: rgba(0, 0, 0, 150);"
            "    border: 1px solid #555;"
            "    border-radius: 10px;"
            "}"
            "QPushButton:hover {"
            "    background-color: rgba(50, 50, 50, 180);"
            "    border: 2px solid #FFD700;"
            "}"
            );
        btn->setCursor(Qt::PointingHandCursor);
        btn->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Expanding);

        QVBoxLayout* btnLayout = new QVBoxLayout(btn);
        btnLayout->setContentsMargins(10, 10, 10, 10);
        btnLayout->setSpacing(5);
        btnLayout->setAlignment(Qt::AlignCenter);

        QLabel* titleLbl = new QLabel(options[i].title, btn);
        titleLbl->setStyleSheet("background: transparent; border: none; font-size: 20px; font-weight: bold; color: #FFD700;");
        titleLbl->setAlignment(Qt::AlignCenter);
        titleLbl->setWordWrap(true);
        titleLbl->setAttribute(Qt::WA_TransparentForMouseEvents);

        QLabel* descLbl = new QLabel(options[i].description, btn);
        descLbl->setStyleSheet("background: transparent; border: none; font-size: 14px; color: white;");
        descLbl->setAlignment(Qt::AlignCenter);
        descLbl->setWordWrap(true);
        descLbl->setAttribute(Qt::WA_TransparentForMouseEvents);

        btnLayout->addWidget(titleLbl);
        btnLayout->addWidget(descLbl);

        connect(btn, &QPushButton::clicked, [this, i]() {
            emit optionSelected(i);
            this->hide();
        });

        optionsLayout->addWidget(btn);
    }

    if (parentWidget()) {
        this->resize(parentWidget()->size());
    }

    this->show();
    this->raise();
}

UpgradeOption LevelUpWidget::getOption(int index) const {
    if (index >= 0 && index < static_cast<int>(currentOptions.size())) {
        return currentOptions[index];
    }
    return UpgradeOption();
}

void LevelUpWidget::paintEvent(QPaintEvent *event) {
    Q_UNUSED(event);
    QPainter painter(this);
    painter.fillRect(this->rect(), QColor(0, 0, 0, 180));
}

void LevelUpWidget::mousePressEvent(QMouseEvent *event) { event->accept(); }
void LevelUpWidget::mouseReleaseEvent(QMouseEvent *event) { event->accept(); }
void LevelUpWidget::mouseMoveEvent(QMouseEvent *event) { event->accept(); }

void LevelUpWidget::resizeEvent(QResizeEvent *event) {
    if (parentWidget()) {
        this->resize(parentWidget()->size());
    }
    QWidget::resizeEvent(event);
}
