#ifndef TUTORIALWIDGET_H
#define TUTORIALWIDGET_H

#include <QWidget>
#include <QLabel>
#include <QPushButton>
#include <QFrame>
#include <QMediaPlayer>
#include <QVideoWidget>
#include <QAudioOutput>
#include <vector>

struct TutorialPage {
    QString title;
    QString text;
    QString mediaPath;
};

class TutorialWidget : public QWidget
{
    Q_OBJECT

public:
    explicit TutorialWidget(QWidget *parent = nullptr);
    ~TutorialWidget();

signals:
    void closed();

protected:
    void keyPressEvent(QKeyEvent *event) override;

private slots:
    void nextPage();
    void prevPage();
    void closeWidget();

private:
    void setupUi();
    void loadTutorialData();
    void updateContent();

    QFrame* containerFrame;
    QLabel* lblTitle;
    QLabel* lblText;
    QLabel* lblPageNumber;
    QLabel* lblImagePlaceholder;

    QVideoWidget* videoWidget;
    QMediaPlayer* mediaPlayer;
    QAudioOutput* audioOutput;

    QPushButton* btnNext;
    QPushButton* btnPrev;
    QPushButton* btnClose;

    std::vector<TutorialPage> pages;
    int currentPageIndex = 0;
};

#endif // TUTORIALWIDGET_H
