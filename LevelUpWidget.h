#ifndef LEVELUPWIDGET_H
#define LEVELUPWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QGraphicsDropShadowEffect>
#include <QPainter>
#include <QMouseEvent>
#include "LevelUpSystem.h"

class LevelUpWidget : public QWidget {
    Q_OBJECT
public:
    explicit LevelUpWidget(QWidget *parent = nullptr) : QWidget(parent) {
        this->setWindowFlags(Qt::FramelessWindowHint);

        QVBoxLayout* overlayLayout = new QVBoxLayout(this);
        overlayLayout->setAlignment(Qt::AlignCenter);

        QWidget* container = new QWidget(this);
        container->setFixedSize(900, 550);
        container->setStyleSheet(
            "QWidget {"
            //"   background-image: url(background.png);"
            "   background-color: #1a1a1a;"
            "   border: 3px solid #D4AF37;"
            "   border-radius: 15px;"
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

    void ShowOptions(const std::vector<UpgradeOption>& options) {
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
                "   background-color: rgba(0, 0, 0, 150);"
                "   border: 1px solid #555;"
                "   border-radius: 10px;"
                "}"
                "QPushButton:hover {"
                "   background-color: rgba(50, 50, 50, 180);"
                "   border: 2px solid #FFD700;"
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
                emit OptionSelected(i);
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

    UpgradeOption GetOption(int index) const {
        if (index >= 0 && index < currentOptions.size()) return currentOptions[index];
        return UpgradeOption();
    }

protected:
    void paintEvent(QPaintEvent *event) override {
        QPainter painter(this);
        painter.fillRect(this->rect(), QColor(0, 0, 0, 180));
    }

    void mousePressEvent(QMouseEvent *event) override { event->accept(); }
    void mouseReleaseEvent(QMouseEvent *event) override { event->accept(); }
    void mouseMoveEvent(QMouseEvent *event) override { event->accept(); }

    void resizeEvent(QResizeEvent *event) override {
        if (parentWidget()) {
            this->resize(parentWidget()->size());
        }
        QWidget::resizeEvent(event);
    }

signals:
    void OptionSelected(int index);

private:
    QHBoxLayout* optionsLayout;
    std::vector<UpgradeOption> currentOptions;
};

#endif // LEVELUPWIDGET_H
