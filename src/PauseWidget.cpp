#include "PauseWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGraphicsDropShadowEffect>

PauseWidget::PauseWidget(QWidget *parent) : QWidget(parent)
{
    setupUi();
}

void PauseWidget::setupUi()
{
    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setObjectName("PauseOverlay");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    containerFrame = new QFrame(this);
    containerFrame->setFixedSize(420, 160);
    containerFrame->setObjectName("Container");

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 150));
    shadow->setOffset(0, 10);
    containerFrame->setGraphicsEffect(shadow);

    mainLayout->addWidget(containerFrame);

    QString styles = R"(
        QWidget#PauseOverlay {
            background-color: rgba(0, 0, 0, 100);
        }

        QFrame#Container {
            background-color: #FFF9C4;
            border: 4px solid #FBC02D;
            border-radius: 20px;
        }

        QLabel {
            font-family: 'Unispace';
            font-size: 20pt;
            font-weight: bold;
            color: #3E2723;
        }

        QPushButton {
            background: transparent;
            border: none;
        }

        QPushButton#BtnContinue {
            border-image: url(:/textures/btn_continue.png) 0 0 0 0 stretch stretch;
        }

        QPushButton#BtnExit {
            border-image: url(:/textures/btn_exit.png) 0 0 0 0 stretch stretch;
        }

        QPushButton:hover {
            margin-top: 2px;
        }
        QPushButton:pressed {
            margin-top: 5px;
        }
    )";
    this->setStyleSheet(styles);

    QVBoxLayout *frameLayout = new QVBoxLayout(containerFrame);
    frameLayout->setContentsMargins(20, 10, 20, 20);
    frameLayout->setSpacing(10);

    labelTitle = new QLabel("Pause", containerFrame);
    labelTitle->setAlignment(Qt::AlignCenter);
    frameLayout->addWidget(labelTitle);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(20);
    buttonsLayout->setAlignment(Qt::AlignCenter);

    btnContinue = new QPushButton(containerFrame);
    btnContinue->setObjectName("BtnContinue");
    btnContinue->setFixedSize(140, 65);
    btnContinue->setCursor(Qt::PointingHandCursor);

    btnExit = new QPushButton(containerFrame);
    btnExit->setObjectName("BtnExit");
    btnExit->setFixedSize(140, 68);
    btnExit->setCursor(Qt::PointingHandCursor);

    buttonsLayout->addWidget(btnContinue);
    buttonsLayout->addWidget(btnExit);

    frameLayout->addLayout(buttonsLayout);

    connect(btnContinue, &QPushButton::clicked, this, &PauseWidget::continueClicked);
    connect(btnExit, &QPushButton::clicked, this, &PauseWidget::exitClicked);
}

void PauseWidget::showEvent(QShowEvent *event)
{
    if(parentWidget()) {
        this->resize(parentWidget()->size());
    }
    QWidget::showEvent(event);
}
