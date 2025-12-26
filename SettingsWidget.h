#ifndef SETTINGSWIDGET_H
#define SETTINGSWIDGET_H

#include <QWidget>
#include <QList>

class QLabel;
class QSlider;
class QPushButton;
class QFrame;

class SettingsWidget : public QWidget
{
    Q_OBJECT

public:
    explicit SettingsWidget(QWidget *parent = nullptr);
    ~SettingsWidget();

    void SetCurrentRadius(int Radius);

signals:
    void MapRadChanged(int NewRad);
    void VolumeChanged(int volume);
    void BackClicked();

private:
    const QList<int> MapRads = {10, 15, 20};
    const QList<QString> MapSizeNames = {"Small", "Medium", "Big"};

    QFrame *m_containerFrame;

    QLabel *m_labelTitle;

    QLabel *m_labelVolume;
    QSlider *m_sldVolume;

    QLabel *m_labelMapSize;
    QSlider *m_sldMapSize;
    QLabel *m_labelCurrentMapSizeName;

    QPushButton *m_btnClose;

    void setupUi();
    void UpdateMapSizeLabel(int sliderValue);

private slots:
    void onBtnCloseClicked();
    void onVolumeValueChanged(int value);
    void onMapSizeValueChanged(int sliderValue);
};

#endif // SETTINGSWIDGET_H
