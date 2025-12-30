#include "MainWindow.h"
#include "SpellManager.h"

#include <QApplication>
#include <QLocale>
#include <QTranslator>
#include <QMainWindow>

int main(int argc, char *argv[])
{
    QApplication a(argc, argv);

    a.setStyleSheet(
        "QMessageBox {"
        "    font-family: 'Unispace';"
        "}"
        "QMessageBox QLabel {"
        "    font-family: 'Unispace';"

        "}"
        "QMessageBox QPushButton {"
        "    font-family: 'Unispace';"
        "}"
        );

     QIcon appIcon("icon.png");
     a.setWindowIcon(appIcon);
     QTranslator translator;
     const QStringList uiLanguages = QLocale::system().uiLanguages();
     for (const QString &locale : uiLanguages) {
         const QString baseName = "BikiniBottom2_" + QLocale(locale).name();
         if (translator.load(":/i18n/" + baseName)) {
             a.installTranslator(&translator);
             break;
         }
    }

    SpellManager::Instance().LoadHeroSpells("hero_spells.json");
    SpellManager::Instance().LoadEnemySpells("enemy_spells.json");

    MainWindow w;
    w.show();

    return a.exec();
}
