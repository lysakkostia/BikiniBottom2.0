#include "GameSelectionWidget.h"
#include <QHeaderView>
#include <QDateTime>
#include <QDebug>
#include <QRegularExpression>
#include <QCoreApplication>
#include <QPainter>
#include <QGraphicsDropShadowEffect>
#include <QFrame>
#include <QScrollBar>

const QString BUTTON_STYLE =
    "QPushButton {"
    "    background-color: #5D4037;"
    "    color: #FFE0B2;"
    "    font-weight: bold;"
    "    border: 2px solid #8D6E63;"
    "    border-radius: 10px;"
    "    font-size: 16px;"
    "    padding: 5px;"
    "}"
    "QPushButton:hover { background-color: #6D4C41; border-color: #FFD54F; color: white; }"
    "QPushButton:pressed { background-color: #3E2723; border-color: #FF6F00; }";

const QString LIST_STYLE =
    "QListWidget {"
    "    background-color: rgba(30, 30, 30, 200);"
    "    border: 3px solid #5D4037;"
    "    border-radius: 10px;"
    "    color: #E0E0E0;"
    "    font-size: 16px;"
    "    padding: 5px;"
    "}"
    "QListWidget::item {"
    "    border-bottom: 1px solid #5D4037;"
    "    padding: 10px;"
    "}"
    "QListWidget::item:selected {"
    "    background-color: #4E342E;"
    "    border: 2px solid #FFD54F;"
    "    border-radius: 5px;"
    "    color: #FFD54F;"
    "}";

const QString INPUT_STYLE =
    "QLineEdit {"
    "    background-color: #3E2723;"
    "    border: 2px solid #8D6E63;"
    "    border-radius: 5px;"
    "    color: #FFD54F;"
    "    font-size: 16px;"
    "    padding: 5px;"
    "}"
    "QLineEdit:focus { border: 2px solid #FFD54F; }";

const QString SLIDER_STYLE =
    "QSlider::groove:horizontal {"
    "    border: 1px solid #5D4037;"
    "    height: 8px;"
    "    background: #3E2723;"
    "    margin: 2px 0;"
    "    border-radius: 4px;"
    "}"
    "QSlider::handle:horizontal {"
    "    background: #FFD54F;"
    "    border: 2px solid #3E2723;"
    "    width: 24px;"
    "    height: 24px;"
    "    margin: -10px 0;"
    "    border-radius: 12px;"
    "}"
    "QSlider::handle:horizontal:hover {"
    "    background: #FFECB3;"
    "}";

GameSelectionWidget::GameSelectionWidget(QWidget *parent)
    : QWidget(parent), creationOverlay(nullptr), messageOverlay(nullptr)
{
    savesDir = QCoreApplication::applicationDirPath() + "/saves";
    QDir dir(savesDir);
    if (!dir.exists()) {
        dir.mkpath(".");
    }

    setupUI();
    setupCreationOverlay();
    refreshSaveList();
}

void GameSelectionWidget::setupUI()
{
    QVBoxLayout* mainLayout = new QVBoxLayout(this);
    mainLayout->setContentsMargins(50, 30, 50, 50);
    mainLayout->setSpacing(20);

    QLabel* title = new QLabel("ВИБІР СВІТУ", this);
    title->setAlignment(Qt::AlignCenter);
    title->setStyleSheet("font-size: 32px; font-weight: bold; color: #FFD54F; font-family: 'Times New Roman'; text-shadow: 2px 2px #000;");
    mainLayout->addWidget(title);

    worldsList = new QListWidget(this);
    worldsList->setStyleSheet(LIST_STYLE);
    worldsList->setVerticalScrollBarPolicy(Qt::ScrollBarAsNeeded);
    worldsList->horizontalScrollBar()->setStyleSheet("QScrollBar {height:0px;}");
    worldsList->verticalScrollBar()->setStyleSheet(
        "QScrollBar:vertical { background: #3E2723; width: 12px; margin: 0px; border-radius: 5px; }"
        "QScrollBar::handle:vertical { background: #8D6E63; min-height: 20px; border-radius: 5px; }"
        "QScrollBar::add-line:vertical { height: 0px; } QScrollBar::sub-line:vertical { height: 0px; }"
        );

    mainLayout->addWidget(worldsList);

    QHBoxLayout* btnLayout = new QHBoxLayout();
    btnLayout->setSpacing(20);

    btnPlay = new QPushButton("ГРАТИ", this);
    btnCreateNew = new QPushButton("НОВИЙ СВІТ", this);
    btnDelete = new QPushButton("ВИДАЛИТИ", this);
    btnBack = new QPushButton("НАЗАД", this);

    btnPlay->setCursor(Qt::PointingHandCursor);
    btnCreateNew->setCursor(Qt::PointingHandCursor);
    btnDelete->setCursor(Qt::PointingHandCursor);
    btnBack->setCursor(Qt::PointingHandCursor);

    btnPlay->setStyleSheet(BUTTON_STYLE);
    btnCreateNew->setStyleSheet(BUTTON_STYLE);
    btnDelete->setStyleSheet(BUTTON_STYLE);
    btnBack->setStyleSheet(BUTTON_STYLE);

    btnPlay->setFixedHeight(50);
    btnCreateNew->setFixedHeight(50);
    btnDelete->setFixedHeight(50);
    btnBack->setFixedHeight(50);

    btnLayout->addWidget(btnPlay);
    btnLayout->addWidget(btnCreateNew);
    btnLayout->addWidget(btnDelete);
    btnLayout->addWidget(btnBack);

    mainLayout->addLayout(btnLayout);

    connect(btnBack, &QPushButton::clicked, this, &GameSelectionWidget::backRequested);
    connect(btnCreateNew, &QPushButton::clicked, [this](){ showCreationMode(true); });

    connect(btnPlay, &QPushButton::clicked, this, &GameSelectionWidget::onPlayClicked);
    connect(btnDelete, &QPushButton::clicked, this, &GameSelectionWidget::onDeleteClicked);
}

void GameSelectionWidget::setupCreationOverlay()
{
    creationOverlay = new QWidget(this);
    creationOverlay->setVisible(false);
    creationOverlay->setStyleSheet("background-color: rgba(0, 0, 0, 150);");

    QVBoxLayout* overlayLayout = new QVBoxLayout(creationOverlay);
    overlayLayout->setAlignment(Qt::AlignCenter);

    QFrame* dialogFrame = new QFrame(creationOverlay);
    dialogFrame->setFixedSize(500, 350);
    dialogFrame->setStyleSheet(
        "QFrame {"
        "    background-color: #2D1E1B;"
        "    border: 3px solid #FFD54F;"
        "    border-radius: 15px;"
        "}"
        );

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(30);
    shadow->setColor(Qt::black);
    shadow->setOffset(0, 10);
    dialogFrame->setGraphicsEffect(shadow);

    overlayLayout->addWidget(dialogFrame);

    QVBoxLayout* formLayout = new QVBoxLayout(dialogFrame);
    formLayout->setSpacing(10);
    formLayout->setContentsMargins(40, 30, 40, 30);

    QLabel* lblTitle = new QLabel("СТВОРЕННЯ СВІТУ", dialogFrame);
    lblTitle->setAlignment(Qt::AlignCenter);
    lblTitle->setStyleSheet("font-size: 24px; font-weight: bold; color: #FFD54F; border: none; background: transparent;");
    formLayout->addWidget(lblTitle);

    QLabel* lblName = new QLabel("Назва світу:", dialogFrame);
    lblName->setStyleSheet("color: #E0E0E0; font-size: 16px; border: none; background: transparent;");
    editWorldName = new QLineEdit(dialogFrame);
    editWorldName->setStyleSheet(INPUT_STYLE);
    editWorldName->setPlaceholderText("Введіть назву...");

    QLabel* lblSeed = new QLabel("Сід генерації (опціонально):", dialogFrame);
    lblSeed->setStyleSheet("color: #E0E0E0; font-size: 16px; border: none; background: transparent;");
    editSeed = new QLineEdit(dialogFrame);
    editSeed->setStyleSheet(INPUT_STYLE);
    editSeed->setPlaceholderText("Порожньо = Випадковий");

    formLayout->addWidget(lblName);
    formLayout->addWidget(editWorldName);
    formLayout->addWidget(lblSeed);
    formLayout->addWidget(editSeed);

    QLabel* lblSizeTitle = new QLabel("Розмір карти:", dialogFrame);
    lblSizeTitle->setStyleSheet("color: #E0E0E0; font-size: 16px; border: none; background: transparent; margin-top: 5px;");
    formLayout->addWidget(lblSizeTitle);

    QHBoxLayout* sliderLayout = new QHBoxLayout();

    sldMapSize = new QSlider(Qt::Horizontal, dialogFrame);
    sldMapSize->setStyleSheet(SLIDER_STYLE);
    sldMapSize->setCursor(Qt::PointingHandCursor);
    sldMapSize->setMinimum(0);
    sldMapSize->setMaximum(mapRads.size() - 1);
    sldMapSize->setTickInterval(1);
    sldMapSize->setValue(1);

    lblMapSizeName = new QLabel("Середня", dialogFrame);
    lblMapSizeName->setFixedWidth(100);
    lblMapSizeName->setAlignment(Qt::AlignCenter);
    lblMapSizeName->setStyleSheet("color: #FFD54F; font-size: 16px; font-weight: bold; border: none; background: transparent;");

    connect(sldMapSize, &QSlider::valueChanged, this, &GameSelectionWidget::updateMapSizeLabel);

    sliderLayout->addWidget(sldMapSize);
    sliderLayout->addWidget(lblMapSizeName);
    formLayout->addLayout(sliderLayout);

    formLayout->addStretch();

    QHBoxLayout* btnBox = new QHBoxLayout();
    btnConfirmCreate = new QPushButton("СТВОРИТИ", dialogFrame);
    btnCancelCreate = new QPushButton("СКАСУВАТИ", dialogFrame);

    btnConfirmCreate->setCursor(Qt::PointingHandCursor);
    btnCancelCreate->setCursor(Qt::PointingHandCursor);

    btnConfirmCreate->setStyleSheet(BUTTON_STYLE);
    btnCancelCreate->setStyleSheet(BUTTON_STYLE);
    btnConfirmCreate->setFixedHeight(45);
    btnCancelCreate->setFixedHeight(45);

    btnBox->addWidget(btnConfirmCreate);
    btnBox->addWidget(btnCancelCreate);
    formLayout->addLayout(btnBox);

    connect(btnCancelCreate, &QPushButton::clicked, [this](){ showCreationMode(false); });
    connect(btnConfirmCreate, &QPushButton::clicked, this, &GameSelectionWidget::onCreateNewClicked);
}

void GameSelectionWidget::updateMapSizeLabel(int index)
{
    if (index >= 0 && index < mapSizeNames.size()) {
        lblMapSizeName->setText(mapSizeNames[index]);
    }
}

void GameSelectionWidget::refreshSaveList()
{
    worldsList->clear();
    QDir dir(savesDir);
    QStringList filters;
    filters << "*.json";
    dir.setNameFilters(filters);

    QFileInfoList list = dir.entryInfoList(QDir::Files | QDir::NoDotAndDotDot, QDir::Time);

    for (const QFileInfo& fileInfo : list) {
        QString displayName = fileInfo.completeBaseName();
        QString date = fileInfo.lastModified().toString("dd.MM.yyyy HH:mm");

        QListWidgetItem* item = new QListWidgetItem(QString("%1\nCreated: %2").arg(displayName, date));
        item->setData(Qt::UserRole, fileInfo.absoluteFilePath());
        worldsList->addItem(item);
    }
}

void GameSelectionWidget::showCreationMode(bool show)
{
    if (!creationOverlay) return;

    if (show) {
        editWorldName->clear();
        editSeed->clear();
        sldMapSize->setValue(1);
        editWorldName->setFocus();
        creationOverlay->resize(this->size());
        creationOverlay->raise();
        creationOverlay->show();
    } else {
        creationOverlay->hide();
    }
}

void GameSelectionWidget::resizeEvent(QResizeEvent *event)
{
    QWidget::resizeEvent(event);
    if (creationOverlay && creationOverlay->isVisible()) {
        creationOverlay->resize(this->size());
    }
    if (messageOverlay && messageOverlay->isVisible()) {
        messageOverlay->resize(this->size());
    }
}

void GameSelectionWidget::paintEvent(QPaintEvent *event)
{
    Q_UNUSED(event);
    QPainter painter(this);
}

void GameSelectionWidget::onCreateNewClicked()
{
    QString name = editWorldName->text().trimmed();
    if (name.isEmpty()) {
        showCustomMessage("ПОМИЛКА", "Введіть назву світу!", false);
        return;
    }

    QRegularExpression re("[\\\\/:*?\"<>|]");
    if (name.contains(re)) {
        showCustomMessage("ПОМИЛКА", "Назва містить заборонені символи!\n( \\ / : * ? \" < > | )", false);
        return;
    }

    QString filename = savesDir + "/" + name + ".json";
    if (QFile::exists(filename)) {
        showCustomMessage("ПОМИЛКА", "Світ з такою назвою вже існує!", false);
        return;
    }

    QString seed = editSeed->text().trimmed();

    int index = sldMapSize->value();
    int radius = mapRads[index];

    emit createNewGameRequested(name, seed, radius);
    showCreationMode(false);
}

void GameSelectionWidget::onPlayClicked()
{
    QListWidgetItem* current = worldsList->currentItem();
    if (!current) {
        showCustomMessage("УВАГА", "Оберіть світ зі списку, щоб почати гру.", false);
        return;
    }

    QString filePath = current->data(Qt::UserRole).toString();
    emit loadGameRequested(filePath);
}

void GameSelectionWidget::onDeleteClicked()
{
    QListWidgetItem* current = worldsList->currentItem();
    if (!current) return;

    QString filePath = current->data(Qt::UserRole).toString();

    showCustomMessage("ВИДАЛЕННЯ", "Ви впевнені, що хочете назавжди видалити цей світ?", true, [this, filePath](){
        QFile::remove(filePath);
        refreshSaveList();
    });
}

void GameSelectionWidget::showCustomMessage(const QString& title, const QString& message, bool isQuestion, std::function<void()> onYes)
{
    if (messageOverlay) {
        messageOverlay->deleteLater();
    }

    messageOverlay = new QWidget(this);
    messageOverlay->resize(this->size());
    messageOverlay->setStyleSheet("background-color: rgba(0, 0, 0, 180);");

    QVBoxLayout* mainLayout = new QVBoxLayout(messageOverlay);
    mainLayout->setAlignment(Qt::AlignCenter);

    QFrame* box = new QFrame(messageOverlay);
    box->setFixedSize(400, 250);
    box->setStyleSheet(
        "QFrame {"
        "   background-color: #3E2723;"
        "   border: 3px solid #FF5722;"
        "   border-radius: 15px;"
        "}"
        );

    QGraphicsDropShadowEffect* shadow = new QGraphicsDropShadowEffect();
    shadow->setBlurRadius(20);
    shadow->setColor(Qt::black);
    shadow->setOffset(0, 5);
    box->setGraphicsEffect(shadow);

    mainLayout->addWidget(box);

    QVBoxLayout* boxLayout = new QVBoxLayout(box);
    boxLayout->setContentsMargins(20,20,20,20);
    boxLayout->setSpacing(10);

    QLabel* lblTitle = new QLabel(title, box);
    lblTitle->setAlignment(Qt::AlignCenter);
    lblTitle->setStyleSheet("font-size: 22px; font-weight: bold; color: #FFAB91; border: none; background: transparent;");

    QLabel* lblMsg = new QLabel(message, box);
    lblMsg->setAlignment(Qt::AlignCenter);
    lblMsg->setWordWrap(true);
    lblMsg->setStyleSheet("font-size: 16px; color: white; border: none; background: transparent;");

    boxLayout->addWidget(lblTitle);
    boxLayout->addWidget(lblMsg);
    boxLayout->addStretch();

    QHBoxLayout* btnLayout = new QHBoxLayout();

    if (isQuestion) {
        QPushButton* btnYes = new QPushButton("ТАК", box);
        QPushButton* btnNo = new QPushButton("НІ", box);
        btnYes->setStyleSheet(BUTTON_STYLE);
        btnNo->setStyleSheet(BUTTON_STYLE);
        btnYes->setCursor(Qt::PointingHandCursor);
        btnNo->setCursor(Qt::PointingHandCursor);

        btnLayout->addWidget(btnYes);
        btnLayout->addWidget(btnNo);

        connect(btnNo, &QPushButton::clicked, [this](){
            messageOverlay->hide();
            messageOverlay->deleteLater();
            messageOverlay = nullptr;
        });

        connect(btnYes, &QPushButton::clicked, [this, onYes](){
            if(onYes) onYes();
            messageOverlay->hide();
            messageOverlay->deleteLater();
            messageOverlay = nullptr;
        });

    } else {
        QPushButton* btnOk = new QPushButton("OK", box);
        btnOk->setStyleSheet(BUTTON_STYLE);
        btnOk->setCursor(Qt::PointingHandCursor);
        btnOk->setFixedSize(120, 40);
        btnLayout->addWidget(btnOk);

        connect(btnOk, &QPushButton::clicked, [this](){
            messageOverlay->hide();
            messageOverlay->deleteLater();
            messageOverlay = nullptr;
        });
    }

    boxLayout->addLayout(btnLayout);

    messageOverlay->raise();
    messageOverlay->show();
}
