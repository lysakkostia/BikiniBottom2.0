#include "Pause.h"
#include "ui_Pause.h"
#include <QApplication>

Pause::Pause(QWidget *parent, GameScene* widget): QDialog(parent), ui(new Ui::Pause), hexWidget(widget)
{
    ui->setupUi(this);

    //стилі
    this->setAttribute(Qt::WA_StyledBackground, true);
    this->setFixedSize( 420, 140 );

    this->setStyleSheet(
   "Pause {"
    "background-image: url(TestBackground.png);"
    "background-repeat: no-repeat;"
    "background-position: center;"
    "background-attachment: fixed;"
    "border-image: url(TestBackground.png) 0 0 0 0 stretch stretch;"
    "}"

    );
}

Pause::~Pause()
{
    delete ui;
}

void Pause::on_Continue_clicked()
{
    accept();
}

void Pause::on_Exit_clicked()
{
    if(hexWidget)
    {
    hexWidget->SaveMapToFile("map.dat");
    }
    QApplication::quit();
}
