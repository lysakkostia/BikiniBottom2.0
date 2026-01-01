#ifndef PAUSEWIDGET_H
#define PAUSEWIDGET_H

#include <QWidget>
#include <QPushButton>
#include <QLabel>
#include <QFrame>
#include <QVBoxLayout>

class PauseWidget : public QWidget
{
    Q_OBJECT
public:
    explicit PauseWidget(QWidget *parent = nullptr);

signals:
    void continueClicked();
    void exitClicked();

protected:
    void showEvent(QShowEvent *event) override;

private:
    void setupUi();

    QFrame* containerFrame;
    QLabel* labelTitle;
    QPushButton* btnContinue;
    QPushButton* btnExit;
};

#endif // PAUSEWIDGET_H
