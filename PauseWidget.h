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
    void ContinueClicked();
    void ExitClicked();

protected:
    void showEvent(QShowEvent *event) override;

private:
    void setupUi();

    QFrame *m_containerFrame;
    QLabel *m_labelTitle;
    QPushButton *m_btnContinue;
    QPushButton *m_btnExit;
};

#endif // PAUSEWIDGET_H
