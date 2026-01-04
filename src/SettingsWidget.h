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

signals:
    void volumeChanged(int volume);
    void backClicked();

private:
    QFrame* containerFrame;

    QLabel* labelTitle;
    QLabel* labelVolume;
    QSlider* sldVolume;
    QPushButton* btnClose;

    void setupUi();

private slots:
    void onBtnCloseClicked();
    void onVolumeValueChanged(int value);
};

#endif // SETTINGSWIDGET_H
