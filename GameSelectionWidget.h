#ifndef GAMESELECTIONWIDGET_H
#define GAMESELECTIONWIDGET_H

#include <QWidget>

class QPushButton;
class QLabel;
class QFrame;

class GameSelectionWidget : public QWidget
{
    Q_OBJECT

public:
    explicit GameSelectionWidget(QWidget *parent = nullptr);
    ~GameSelectionWidget();

signals:
    void startNewGameClicked();
    void loadGameClicked();
    void backClicked();

private:
    void setupUi();

    QFrame* containerFrame;
    QLabel* labelTitle;
    QPushButton* btnNewGame;
    QPushButton* btnLoadGame;
    QPushButton* btnBack;
};

#endif // GAMESELECTIONWIDGET_H
