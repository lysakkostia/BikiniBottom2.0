#ifndef PAUSE_H
#define PAUSE_H
#include "GameScene.h"
#include <QDialog>

namespace Ui {
class Pause;
}

class Pause : public QDialog
{
    Q_OBJECT

public:
    explicit Pause(QWidget *parent = nullptr, GameScene *widget = nullptr);
    ~Pause();

private:
    Ui::Pause *ui;
    GameScene *hexWidget;

private slots:
    void on_Continue_clicked();
    void on_Exit_clicked();
};

#endif // PAUSE_H
