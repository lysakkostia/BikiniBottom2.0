#include "SettingsWidget.h".h"

#include <QLabel>
#include <QSlider>
#include <QPushButton>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QGridLayout>
#include <QFrame>
#include <QGraphicsDropShadowEffect>

SettingsWidget::SettingsWidget(QWidget *parent)
    : QWidget(parent)
{
    setupUi();

    if(sldMapSize)
    {
        sldMapSize->setMinimum(0);
        sldMapSize->setMaximum(mapRads.size() - 1);
        sldMapSize->setTickInterval(1);
        sldMapSize->setPageStep(1);
    }
}

SettingsWidget::~SettingsWidget(){}

void SettingsWidget::setupUi()
{
    this->setAttribute(Qt::WA_StyledBackground, true);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);

    containerFrame = new QFrame(this);
    containerFrame->setFixedSize(500, 400);
    containerFrame->setObjectName("Container");

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(30);
    shadow->setColor(QColor(0, 0, 0, 100));
    shadow->setOffset(0, 10);
    containerFrame->setGraphicsEffect(shadow);

    mainLayout->addWidget(containerFrame);

    QString styles = R"(
        QFrame#Container {
            background-color: #FFF9C4;
            border: 4px solid #FBC02D;
            border-radius: 20px;
        }

        QLabel {
            font-family: 'Unispace';
            font-size: 16pt;
            font-weight: bold;
            color: #5D4037;
        }

        QLabel#Title {
            font-size: 26pt;
            color: #0277BD;
            margin-bottom: 10px;
        }

        QSlider::groove:horizontal {
            border: 1px solid #F9A825;
            height: 10px;
            background: #FFF59D;
            margin: 2px 0;
            border-radius: 5px;
        }

        QSlider::handle:horizontal {
            background: #0277BD;
            border: 2px solid #E1F5FE;
            width: 26px;
            height: 26px;
            border-radius: 13px;
            margin: -8px 0;
        }

        QSlider::handle:horizontal:hover {
            background: #039BE5;
            width: 28px;
            height: 28px;
            margin: -9px 0;
            border-radius: 14px;
        }

        QPushButton {
            border-image: url(btn_return.png) 0 0 0 0 stretch stretch;
            background: transparent;
        }
        QPushButton:hover {
            margin-top: 2px;
            margin-left: 2px;
        }
        QPushButton:pressed {
            margin-top: 4px;
            margin-left: 4px;
        }
    )";

    this->setStyleSheet(styles);

    QVBoxLayout *containerLayout = new QVBoxLayout(containerFrame);
    containerLayout->setContentsMargins(40, 30, 40, 40);
    containerLayout->setSpacing(10);

    labelTitle = new QLabel("SETTINGS", containerFrame);
    labelTitle->setObjectName("Title");
    labelTitle->setAlignment(Qt::AlignCenter);
    containerLayout->addWidget(labelTitle);

    containerLayout->addSpacing(10);

    QGridLayout *settingsGrid = new QGridLayout();
    settingsGrid->setVerticalSpacing(25);
    settingsGrid->setHorizontalSpacing(20);

    labelVolume = new QLabel("Volume", containerFrame);
    labelVolume->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    settingsGrid->addWidget(labelVolume, 0, 0);

    sldVolume = new QSlider(Qt::Horizontal, containerFrame);
    sldVolume->setCursor(Qt::PointingHandCursor);
    sldVolume->setMinimumHeight(40);
    settingsGrid->addWidget(sldVolume, 0, 1);

    labelMapSize = new QLabel("Map Size", containerFrame);
    labelMapSize->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    settingsGrid->addWidget(labelMapSize, 1, 0);

    sldMapSize = new QSlider(Qt::Horizontal, containerFrame);
    sldMapSize->setCursor(Qt::PointingHandCursor);
    sldMapSize->setMinimumHeight(40);
    settingsGrid->addWidget(sldMapSize, 1, 1);

    labelCurrentMapSizeName = new QLabel("Small", containerFrame);
    labelCurrentMapSizeName->setAlignment(Qt::AlignCenter);
    labelCurrentMapSizeName->setStyleSheet("color: #0277BD; font-size: 14pt;");
    settingsGrid->addWidget(labelCurrentMapSizeName, 2, 1);

    containerLayout->addLayout(settingsGrid);

    containerLayout->addStretch();

    btnClose = new QPushButton(containerFrame);
    btnClose->setFixedSize(140, 55);
    btnClose->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(btnClose);
    btnLayout->addStretch();

    containerLayout->addLayout(btnLayout);

    connect(btnClose, &QPushButton::clicked, this, &SettingsWidget::onBtnCloseClicked);
    connect(sldVolume, &QSlider::valueChanged, this, &SettingsWidget::onVolumeValueChanged);
    connect(sldMapSize, &QSlider::valueChanged, this, &SettingsWidget::onMapSizeValueChanged);
}

void SettingsWidget::onBtnCloseClicked()
{
    emit backClicked();
    this->hide();
}

void SettingsWidget::onVolumeValueChanged(int value)
{
    emit volumeChanged(value);
}

void SettingsWidget::updateMapSizeLabel(int sliderValue)
{
    if(labelCurrentMapSizeName && sliderValue >= 0 && sliderValue < mapSizeNames.size())
        labelCurrentMapSizeName->setText(mapSizeNames.at(sliderValue));
}

void SettingsWidget::setCurrentRadius(int Radius)
{
    int sliderValue = mapRads.indexOf(Radius);
    if(sliderValue == -1)
        sliderValue = 1;

    if(sldMapSize)
    {
        bool OldSignalState = sldMapSize->blockSignals(true);
        sldMapSize->setValue(sliderValue);
        sldMapSize->blockSignals(OldSignalState);
    }
    updateMapSizeLabel(sliderValue);
}

void SettingsWidget::onMapSizeValueChanged(int sliderValue)
{
    if(sliderValue >= 0 && sliderValue < mapRads.size())
    {
        int ActualRadius = mapRads.at(sliderValue);
        emit mapRadChanged(ActualRadius);
        updateMapSizeLabel(sliderValue);
    }
}
