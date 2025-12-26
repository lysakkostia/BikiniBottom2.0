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
    void StartNewGameClicked();
    void LoadGameClicked();
    void BackClicked();

private:
    void setupUi();

    QFrame *m_containerFrame;
    QLabel *m_labelTitle;
    QPushButton *m_btnNewGame;
    QPushButton *m_btnLoadGame;
    QPushButton *m_btnBack;
};

#endif // GAMESELECTIONWIDGET_H
