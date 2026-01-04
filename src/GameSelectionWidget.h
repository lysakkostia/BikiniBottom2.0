#ifndef GAMESELECTIONWIDGET_H
#define GAMESELECTIONWIDGET_H

#include <QWidget>
#include <QListWidget>
#include <QPushButton>
#include <QLineEdit>
#include <QLabel>
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QDir>
#include <functional>

class GameSelectionWidget : public QWidget
{
    Q_OBJECT
public:
    explicit GameSelectionWidget(QWidget *parent = nullptr);
    void refreshSaveList();

protected:
    void paintEvent(QPaintEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

signals:
    void loadGameRequested(const QString& filename);
    void createNewGameRequested(const QString& worldName, const QString& seed, int mapRadius);
    void backRequested();

private slots:
    void onCreateNewClicked();
    void onPlayClicked();
    void onDeleteClicked();

private:
    QListWidget* worldsList;
    QPushButton* btnPlay;
    QPushButton* btnCreateNew;
    QPushButton* btnDelete;
    QPushButton* btnBack;

    QWidget* creationOverlay;
    QLineEdit* editWorldName;
    QLineEdit* editSeed;
    QSlider* sldMapSize;
    QLabel* lblMapSizeName;
    QPushButton* btnConfirmCreate;
    QPushButton* btnCancelCreate;

    QWidget* messageOverlay;

    QString savesDir;
    const QList<int> mapRads = {10, 15, 20};
    const QList<QString> mapSizeNames = {"Маленька", "Середня", "Велика"};

    void setupUI();
    void setupCreationOverlay();
    void showCreationMode(bool show);
    void updateMapSizeLabel(int index);

    void showCustomMessage(const QString& title, const QString& message, bool isQuestion, std::function<void()> onYes = nullptr);
};

#endif // GAMESELECTIONWIDGET_H
