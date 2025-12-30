#ifndef LEVELUPWIDGET_H
#define LEVELUPWIDGET_H

#include <QWidget>
#include <vector>
#include "LevelUpSystem.h"

class QVBoxLayout;
class QHBoxLayout;
class QLabel;
class QPushButton;
class QGraphicsDropShadowEffect;

class LevelUpWidget : public QWidget {
    Q_OBJECT
public:
    explicit LevelUpWidget(QWidget *parent = nullptr);
    void ShowOptions(const std::vector<UpgradeOption>& options);
    UpgradeOption GetOption(int index) const;

signals:
    void OptionSelected(int index);

protected:
    void paintEvent(QPaintEvent *event) override;
    void mousePressEvent(QMouseEvent *event) override;
    void mouseReleaseEvent(QMouseEvent *event) override;
    void mouseMoveEvent(QMouseEvent *event) override;
    void resizeEvent(QResizeEvent *event) override;

private:
    QHBoxLayout* optionsLayout;
    std::vector<UpgradeOption> currentOptions;
};

#endif // LEVELUPWIDGET_H
