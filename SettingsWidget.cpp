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

    if(m_sldMapSize)
    {
        m_sldMapSize->setMinimum(0);
        m_sldMapSize->setMaximum(MapRads.size() - 1);
        m_sldMapSize->setTickInterval(1);
        m_sldMapSize->setPageStep(1);
    }
}

SettingsWidget::~SettingsWidget(){}

void SettingsWidget::setupUi()
{
    this->setAttribute(Qt::WA_StyledBackground, true);

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);

    m_containerFrame = new QFrame(this);
    m_containerFrame->setFixedSize(500, 400);
    m_containerFrame->setObjectName("Container");

    QGraphicsDropShadowEffect *shadow = new QGraphicsDropShadowEffect(this);
    shadow->setBlurRadius(30);
    shadow->setColor(QColor(0, 0, 0, 100));
    shadow->setOffset(0, 10);
    m_containerFrame->setGraphicsEffect(shadow);

    mainLayout->addWidget(m_containerFrame);

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

    QVBoxLayout *containerLayout = new QVBoxLayout(m_containerFrame);
    containerLayout->setContentsMargins(40, 30, 40, 40);
    containerLayout->setSpacing(10);

    m_labelTitle = new QLabel("SETTINGS", m_containerFrame);
    m_labelTitle->setObjectName("Title");
    m_labelTitle->setAlignment(Qt::AlignCenter);
    containerLayout->addWidget(m_labelTitle);

    containerLayout->addSpacing(10);

    QGridLayout *settingsGrid = new QGridLayout();
    settingsGrid->setVerticalSpacing(25);
    settingsGrid->setHorizontalSpacing(20);

    m_labelVolume = new QLabel("Volume", m_containerFrame);
    m_labelVolume->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    settingsGrid->addWidget(m_labelVolume, 0, 0);

    m_sldVolume = new QSlider(Qt::Horizontal, m_containerFrame);
    m_sldVolume->setCursor(Qt::PointingHandCursor);
    m_sldVolume->setMinimumHeight(40);
    settingsGrid->addWidget(m_sldVolume, 0, 1);

    m_labelMapSize = new QLabel("Map Size", m_containerFrame);
    m_labelMapSize->setAlignment(Qt::AlignLeft | Qt::AlignVCenter);
    settingsGrid->addWidget(m_labelMapSize, 1, 0);

    m_sldMapSize = new QSlider(Qt::Horizontal, m_containerFrame);
    m_sldMapSize->setCursor(Qt::PointingHandCursor);
    m_sldMapSize->setMinimumHeight(40);
    settingsGrid->addWidget(m_sldMapSize, 1, 1);

    m_labelCurrentMapSizeName = new QLabel("Small", m_containerFrame);
    m_labelCurrentMapSizeName->setAlignment(Qt::AlignCenter);
    m_labelCurrentMapSizeName->setStyleSheet("color: #0277BD; font-size: 14pt;");
    settingsGrid->addWidget(m_labelCurrentMapSizeName, 2, 1);

    containerLayout->addLayout(settingsGrid);

    containerLayout->addStretch();

    m_btnClose = new QPushButton(m_containerFrame);
    m_btnClose->setFixedSize(140, 55);
    m_btnClose->setCursor(Qt::PointingHandCursor);

    QHBoxLayout *btnLayout = new QHBoxLayout();
    btnLayout->addStretch();
    btnLayout->addWidget(m_btnClose);
    btnLayout->addStretch();

    containerLayout->addLayout(btnLayout);

    connect(m_btnClose, &QPushButton::clicked, this, &SettingsWidget::onBtnCloseClicked);
    connect(m_sldVolume, &QSlider::valueChanged, this, &SettingsWidget::onVolumeValueChanged);
    connect(m_sldMapSize, &QSlider::valueChanged, this, &SettingsWidget::onMapSizeValueChanged);
}

void SettingsWidget::onBtnCloseClicked()
{
    emit BackClicked();
    this->hide();
}

void SettingsWidget::onVolumeValueChanged(int value)
{
    emit VolumeChanged(value);
}

void SettingsWidget::UpdateMapSizeLabel(int sliderValue)
{
    if(m_labelCurrentMapSizeName && sliderValue >= 0 && sliderValue < MapSizeNames.size())
        m_labelCurrentMapSizeName->setText(MapSizeNames.at(sliderValue));
}

void SettingsWidget::SetCurrentRadius(int Radius)
{
    int sliderValue = MapRads.indexOf(Radius);
    if(sliderValue == -1)
        sliderValue = 1;

    if(m_sldMapSize)
    {
        bool OldSignalState = m_sldMapSize->blockSignals(true);
        m_sldMapSize->setValue(sliderValue);
        m_sldMapSize->blockSignals(OldSignalState);
    }
    UpdateMapSizeLabel(sliderValue);
}

void SettingsWidget::onMapSizeValueChanged(int sliderValue)
{
    if(sliderValue >= 0 && sliderValue < MapRads.size())
    {
        int ActualRadius = MapRads.at(sliderValue);
        emit MapRadChanged(ActualRadius);
        UpdateMapSizeLabel(sliderValue);
    }
}
