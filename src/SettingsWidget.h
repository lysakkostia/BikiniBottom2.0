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

    void setCurrentRadius(int Radius);

signals:
    void mapRadChanged(int NewRad);
    void volumeChanged(int volume);
    void backClicked();

private:
    const QList<int> mapRads = {10, 15, 20};
    const QList<QString> mapSizeNames = {"Small", "Medium", "Big"};

    QFrame* containerFrame;

    QLabel* labelTitle;
    QLabel* labelVolume;
    QSlider* sldVolume;
    QLabel* labelMapSize;
    QSlider* sldMapSize;
    QLabel* labelCurrentMapSizeName;
    QPushButton* btnClose;

    void setupUi();
    void updateMapSizeLabel(int sliderValue);

private slots:
    void onBtnCloseClicked();
    void onVolumeValueChanged(int value);
    void onMapSizeValueChanged(int sliderValue);
};

#endif // SETTINGSWIDGET_H
