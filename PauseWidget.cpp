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

    m_containerFrame = new QFrame(this);
    m_containerFrame->setFixedSize(420, 160);
    m_containerFrame->setObjectName("Container");

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(20);
    shadow->setColor(QColor(0, 0, 0, 150));
    shadow->setOffset(0, 10);
    m_containerFrame->setGraphicsEffect(shadow);

    mainLayout->addWidget(m_containerFrame);

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
            border-image: url(btn_continue.png) 0 0 0 0 stretch stretch;
        }

        QPushButton#BtnExit {
            border-image: url(btn_exit.png) 0 0 0 0 stretch stretch;
        }

        QPushButton:hover {
            margin-top: 2px;
        }
        QPushButton:pressed {
            margin-top: 5px;
        }
    )";
    this->setStyleSheet(styles);

    QVBoxLayout *frameLayout = new QVBoxLayout(m_containerFrame);
    frameLayout->setContentsMargins(20, 10, 20, 20);
    frameLayout->setSpacing(10);

    m_labelTitle = new QLabel("Pause", m_containerFrame);
    m_labelTitle->setAlignment(Qt::AlignCenter);
    frameLayout->addWidget(m_labelTitle);

    QHBoxLayout *buttonsLayout = new QHBoxLayout();
    buttonsLayout->setSpacing(20);
    buttonsLayout->setAlignment(Qt::AlignCenter);

    m_btnContinue = new QPushButton(m_containerFrame);
    m_btnContinue->setObjectName("BtnContinue");
    m_btnContinue->setFixedSize(140, 65);
    m_btnContinue->setCursor(Qt::PointingHandCursor);

    m_btnExit = new QPushButton(m_containerFrame);
    m_btnExit->setObjectName("BtnExit");
    m_btnExit->setFixedSize(140, 68);
    m_btnExit->setCursor(Qt::PointingHandCursor);

    buttonsLayout->addWidget(m_btnContinue);
    buttonsLayout->addWidget(m_btnExit);

    frameLayout->addLayout(buttonsLayout);

    connect(m_btnContinue, &QPushButton::clicked, this, &PauseWidget::ContinueClicked);
    connect(m_btnExit, &QPushButton::clicked, this, &PauseWidget::ExitClicked);
}

void PauseWidget::showEvent(QShowEvent *event)
{
    if(parentWidget()) {
        this->resize(parentWidget()->size());
    }
    QWidget::showEvent(event);
}
