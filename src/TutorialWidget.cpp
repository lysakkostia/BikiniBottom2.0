#include "TutorialWidget.h"
#include <QVBoxLayout>
#include <QHBoxLayout>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <QKeyEvent>
#include <QUrl>
#include <QDebug>

TutorialWidget::TutorialWidget(QWidget *parent) : QWidget(parent)
{
    loadTutorialData();
    setupUi();

    mediaPlayer = new QMediaPlayer(this);
    audioOutput = new QAudioOutput(this);

    mediaPlayer->setAudioOutput(audioOutput);
    mediaPlayer->setVideoOutput(videoWidget);

    mediaPlayer->setLoops(QMediaPlayer::Infinite);
    audioOutput->setVolume(0);

    updateContent();
}

TutorialWidget::~TutorialWidget()
{
    if (mediaPlayer) {
        mediaPlayer->stop();
    }
}

void TutorialWidget::loadTutorialData()
{
    QFile file(":/tutorial/tutorial_data.json");

    if (!file.open(QIODevice::ReadOnly | QIODevice::Text)) {
        qWarning() << "Could not open tutorial data file!";
        pages.push_back({"Error", "Could not load tutorial_data.json", ""});
        return;
    }

    QByteArray jsonData = file.readAll();
    file.close();

    QJsonDocument doc = QJsonDocument::fromJson(jsonData);
    if (!doc.isArray()) {
        qWarning() << "JSON root is not an array!";
        return;
    }

    QJsonArray array = doc.array();
    pages.clear();

    for (const QJsonValue &val : array) {
        if (val.isObject()) {
            QJsonObject obj = val.toObject();
            TutorialPage page;
            page.title = obj["title"].toString();
            page.text = obj["text"].toString();
            page.mediaPath = obj["media"].toString();
            pages.push_back(page);
        }
    }
}

void TutorialWidget::setupUi()
{
    this->resize(parentWidget()->size());
    this->setStyleSheet("background-color: rgba(0, 0, 0, 150);");

    QVBoxLayout *mainLayout = new QVBoxLayout(this);
    mainLayout->setAlignment(Qt::AlignCenter);

    containerFrame = new QFrame(this);
    containerFrame->setFixedSize(700, 600);

    QString styles = R"(
        QFrame {
            background-color: #FFF9C4;
            border: 4px solid #FBC02D;
            border-radius: 20px;
        }
        QLabel {
            color: #3E2723;
            font-family: 'Unispace';
            border: none;
            background: transparent;
        }
        QPushButton {
            background-color: #FBC02D;
            border: 2px solid #F57F17;
            border-radius: 10px;
            font-weight: bold;
            font-family: 'Unispace';
            color: #3E2723;
            padding: 5px;
        }
        QPushButton:hover {
            background-color: #FFEE58;
            margin-top: 2px;
        }
        QPushButton:pressed {
            background-color: #F9A825;
            margin-top: 4px;
        }
        QPushButton:disabled {
            background-color: #E0E0E0;
            border-color: #BDBDBD;
            color: #9E9E9E;
        }
    )";
    containerFrame->setStyleSheet(styles);

    QVBoxLayout *frameLayout = new QVBoxLayout(containerFrame);
    frameLayout->setSpacing(15);
    frameLayout->setContentsMargins(30, 20, 30, 20);

    lblTitle = new QLabel("Tutorial", containerFrame);
    lblTitle->setStyleSheet("font-size: 28px; font-weight: bold;");
    lblTitle->setAlignment(Qt::AlignCenter);

    lblImagePlaceholder = new QLabel(containerFrame);
    lblImagePlaceholder->setAlignment(Qt::AlignCenter);
    lblImagePlaceholder->setScaledContents(true);
    lblImagePlaceholder->setFixedSize(600, 300);
    lblImagePlaceholder->setStyleSheet("border: 2px solid #8D6E63; border-radius: 10px;");

    videoWidget = new QVideoWidget(containerFrame);
    videoWidget->setFixedSize(600, 300);

    lblText = new QLabel("Description text...", containerFrame);
    lblText->setWordWrap(true);
    lblText->setAlignment(Qt::AlignCenter);
    lblText->setStyleSheet("font-size: 16px;");
    lblText->setFixedHeight(100);

    QHBoxLayout *navLayout = new QHBoxLayout();

    btnPrev = new QPushButton("<<", containerFrame);
    btnPrev->setFixedSize(60, 40);

    lblPageNumber = new QLabel("1 / 1", containerFrame);
    lblPageNumber->setAlignment(Qt::AlignCenter);
    lblPageNumber->setStyleSheet("font-size: 18px; font-weight: bold;");

    btnNext = new QPushButton(">>", containerFrame);
    btnNext->setFixedSize(60, 40);

    navLayout->addWidget(btnPrev);
    navLayout->addStretch();
    navLayout->addWidget(lblPageNumber);
    navLayout->addStretch();
    navLayout->addWidget(btnNext);

    btnClose = new QPushButton("CLOSE", containerFrame);
    btnClose->setFixedHeight(45);
    btnClose->setCursor(Qt::PointingHandCursor);

    frameLayout->addWidget(lblTitle);

    QHBoxLayout* mediaLayout = new QHBoxLayout();
    mediaLayout->setAlignment(Qt::AlignCenter);
    mediaLayout->addWidget(lblImagePlaceholder);
    mediaLayout->addWidget(videoWidget);
    videoWidget->hide();

    frameLayout->addLayout(mediaLayout);
    frameLayout->addWidget(lblText);
    frameLayout->addLayout(navLayout);
    frameLayout->addWidget(btnClose);

    mainLayout->addWidget(containerFrame);

    connect(btnNext, &QPushButton::clicked, this, &TutorialWidget::nextPage);
    connect(btnPrev, &QPushButton::clicked, this, &TutorialWidget::prevPage);
    connect(btnClose, &QPushButton::clicked, this, &TutorialWidget::closeWidget);
}

void TutorialWidget::updateContent()
{
    if (pages.empty()) return;

    mediaPlayer->stop();
    mediaPlayer->setSource(QUrl());

    const auto& page = pages[currentPageIndex];

    lblTitle->setText(page.title);
    lblText->setText(page.text);
    lblPageNumber->setText(QString("%1 / %2").arg(currentPageIndex + 1).arg(pages.size()));

    QString mediaPath = page.mediaPath;

    if (mediaPath.endsWith(".mp4", Qt::CaseInsensitive) ||
        mediaPath.endsWith(".avi", Qt::CaseInsensitive))
    {
        lblImagePlaceholder->hide();
        videoWidget->show();

        if (mediaPath.startsWith("qrc:/")) {
            mediaPlayer->setSource(QUrl(mediaPath));
        } else {
            mediaPlayer->setSource(QUrl::fromLocalFile(mediaPath));
        }

        mediaPlayer->play();
    }
    else
    {
        videoWidget->hide();
        lblImagePlaceholder->show();

        QPixmap pix(mediaPath);
        if (!pix.isNull()) {
            lblImagePlaceholder->setPixmap(pix.scaled(lblImagePlaceholder->size(),
                                                      Qt::KeepAspectRatio,
                                                      Qt::SmoothTransformation));
        } else {
            lblImagePlaceholder->setText("Image not found: " + mediaPath);
        }
    }

    btnPrev->setEnabled(currentPageIndex > 0);
    btnNext->setEnabled(currentPageIndex < pages.size() - 1);
}

void TutorialWidget::nextPage()
{
    if (currentPageIndex < pages.size() - 1) {
        currentPageIndex++;
        updateContent();
    }
}

void TutorialWidget::prevPage()
{
    if (currentPageIndex > 0) {
        currentPageIndex--;
        updateContent();
    }
}

void TutorialWidget::closeWidget()
{
    mediaPlayer->stop();
    this->hide();
    if (parentWidget()) {
        parentWidget()->setFocus();
    }
    emit closed();
}

void TutorialWidget::keyPressEvent(QKeyEvent *event)
{
    if (event->key() == Qt::Key_Left) {
        prevPage();
    } else if (event->key() == Qt::Key_Right) {
        nextPage();
    } else if (event->key() == Qt::Key_Escape) {
        closeWidget();
    } else {
        QWidget::keyPressEvent(event);
    }
}
