#include "GameScene.h"
#include "GameConstants.h"
#include "Fight.h"
#include "AI.h"
#include "TextureManager.h"
#include <QMessageBox>
#include <QGraphicsView>
#include <QDebug>

namespace Const_Scale = GlobalConst::TextureScale;

GameScene::GameScene(int NRadius, QObject *parent)
    : QGraphicsScene(parent), Map(NRadius > 0 ? NRadius : 10), Hero(QPoint(0,0))
{
    Map.UpdateVisibility(Hero.GetPosition());
    generateMapItems();
    movementTimer = new QTimer(this);
    connect(movementTimer, &QTimer::timeout, this, &GameScene::processStep);
    isMoving = false;
}

void GameScene::generateMapItems()
{
    this->clear();

    const auto& Grid = Map.GetMap();
    for (const auto& Col : Grid)
    {
        for (const auto& Hex_ : Col)
        {
            Hex* modelHex = const_cast<Hex*>(&Hex_);

            HexItem* item = new HexItem(modelHex, this);
            this->addItem(item);
            item->updateZValue();
        }
    }

    QRectF realMapRect = this->itemsBoundingRect();
    qreal margin = 2000.0;
    this->setSceneRect(realMapRect.adjusted(-margin, -margin, margin, margin));
}

void GameScene::refreshMap()
{
    this->update();
}

QWidget* GameScene::getViewWidget()
{
    if (!views().isEmpty()) {
        return views().first();
    }
    return nullptr;
}

bool GameScene::tryMoveHeroTo(const QPoint& targetHexCoords)
{
    QPoint currentPos = Hero.GetPosition();
    const Hex& currentHex = Map.GetQPointLoc(currentPos);
    const Hex& targetHex = Map.GetQPointLoc(targetHexCoords);

    if (!currentHex.IsNeighbor(targetHex)) return false;

    if (targetHex.HaveUnit()) {
        Unit* u = targetHex.GetUnit();
        if (u && u->GetType() == UnitType::StructUnBreak) return false;
    }

    Hero.SetPosition(targetHexCoords);

    const Hex& newHexLocation = Map.GetQPointLoc(Hero.GetPosition());
    if (newHexLocation.HaveUnit()) {
        interactWithContentOnHex(newHexLocation, currentPos);
    }

    foreach (QGraphicsItem *item, items()) {
        HexItem *hexItem = dynamic_cast<HexItem*>(item);
        if (hexItem) {
            hexItem->updateZValue();
            //hexItem->update();
        }
    }

    return true;
}

void GameScene::interactWithContentOnHex(const Hex& hex, const QPoint& previousPos)
{
    Unit* unit = hex.GetUnit();
    if (!unit) return;

    if (unit->IsEnemy())
    {
        processCombat(unit, previousPos);
    }
    else if (unit->GetType() == UnitType::Friend)
    {
        processFriendly(unit);
    }
    else if (unit->GetType() == UnitType::CampfireUnit)
    {
        processCampfire(unit);
    }
    else if (unit->GetType() == UnitType::StructBreak)
    {
        processTreasure(unit);
    }
}

void GameScene::processCombat(Unit* enemy, const QPoint& previousPos)
{
    qWarning("Hero moved onto an enemy hex! Starting fight.");

    QWidget* parentView = getViewWidget();

    Fight fightDialog(TextureManager::GetInstance().getUnitTexture(enemy->GetType()), &Hero, enemy, parentView);
    int fightResultCode = fightDialog.exec();

    bool playerEscaped = fightDialog.didPlayerEscaped();

    if (fightResultCode == QDialog::Accepted)
    {
        qDebug("Fight won!");
        Map.ClearUnitAt(Hero.GetPosition());
        Hero.LevelUp();
        Map.DecrementEnemyCount();

        if (Map.GetEnemyCount() <= 0) {
            QMessageBox::information(parentView, tr("Victory!"), tr("Congratulations! You have defeated all enemies!"));
            emit victory();
        }
    }
    else
    {
        if (Hero.GetHP() <= 0) {
            qDebug("Fight lost. Game Over.");
            emit gameOver();
        }
        else {
            qDebug() << (playerEscaped ? "Hero escaped" : "Dialog closed");
            Hero.SetPosition(previousPos);
        }
    }
}

void GameScene::processCampfire(Unit* campfireUnit)
{
    QWidget* parentView = getViewWidget();

    CampfireUnit* campfire = dynamic_cast<CampfireUnit*>(campfireUnit);
    if (!campfire || !campfire->GetAI()) return;

    Campfire* campfireAI = dynamic_cast<Campfire*>(campfire->GetAI());
    if (!campfireAI) return;

    double oldHP = Hero.GetHP();
    double oldMana = Hero.GetMana();

    campfireAI->Heal(&Hero);

    float currentCharges = campfire->GetHP();
    campfire->SetHP(currentCharges - 1);

    QString msg = tr("You rest at the campfire.\nHP: %1 -> %2\nMana: %3 -> %4\nRemaining uses: %5")
                      .arg(oldHP).arg(Hero.GetHP())
                      .arg(oldMana).arg(Hero.GetMana())
                      .arg(campfire->GetHP());

    QMessageBox::information(parentView, tr("Campfire"), msg);

    if (campfire->GetHP() <= 0) {
        QMessageBox::information(parentView, tr("Campfire"), tr("The campfire has extinguished."));
        Map.ClearUnitAt(Hero.GetPosition());
    }
}

void GameScene::processFriendly(Unit* friendUnit)
{
    QWidget* parentView = getViewWidget();

    if (friendUnit->GetAI()) {
        Friendly* friendlyAI = dynamic_cast<Friendly*>(friendUnit->GetAI());
        if (friendlyAI) {
            QMessageBox::information(parentView, tr("Friendly NPC"), QString::fromStdString(friendlyAI->getGreeting()));
        }
    }
}

void GameScene::processTreasure(Unit* treasureUnit)
{
    QWidget* parentView = getViewWidget();
    QMessageBox::information(parentView, tr("Treasure Chest"), tr("You found a chest! Level Up!"));
    Hero.LevelUp();
    Map.ClearUnitAt(Hero.GetPosition());
}

void GameScene::handleHexClick(HexItem* item)
{
    if (IsHeroMoving()) return;

    Hex* targetHex = item->getModelHex();
    QPoint targetPos(targetHex->GetQR().first, targetHex->GetQR().second);

    if (tryMoveHeroTo(targetPos))
    {
        Map.UpdateVisibility(Hero.GetPosition());

        this->update();
        emit heroStatsChanged();
    }

    if (targetPos == pendingTarget)
    {
        StartMovement();
        clearPathHighlight();
        pendingTarget = QPoint(-999, -999);
    }
    else
    {
        clearPathHighlight();

        currentPath = Map.FindPath(Hero.GetPosition(), targetPos);

        if (!currentPath.empty()) {
            highlightPath();
            pendingTarget = targetPos;
        } else {
            pendingTarget = QPoint(-999, -999);
        }
    }
}

Hex* GameScene::getHeroHex()
{
    return const_cast<Hex*>(&Map.GetQPointLoc(Hero.GetPosition()));
}

void GameScene::SaveMapToFile(const QString& filePath)
{
    HeroStats CurrentStats = GetStats();
    Map.SaveToFile(filePath, Hero.GetPosition(), CurrentStats.HP, CurrentStats.MP, CurrentStats.LVL);
}

bool GameScene::LoadMapFromFile(const QString& filePath)
{
    QPoint heroPos;
    double LoadedHeroHP, LoadedHeroMP, LoadedHeroLVL;
    bool Success = Map.LoadFromFile(filePath, heroPos, LoadedHeroHP, LoadedHeroMP, LoadedHeroLVL);

    if(Success)
    {
        Hero.SetPosition(heroPos);
        Hero.SetLevel(LoadedHeroLVL);
        Hero.RecalculateStats();
        Hero.SetHP(LoadedHeroHP);
        Hero.SetMana(LoadedHeroMP);
        Map.UpdateVisibility(Hero.GetPosition());
        generateMapItems();
        emit heroStatsChanged();
    }
    return Success;
}

GameScene::HeroStats GameScene::GetStats()
{
    HeroStats stats;
    stats.HP = Hero.GetHP();
    stats.MP = Hero.GetMana();
    stats.LVL = Hero.GetLevel();
    return stats;
}

void GameScene::setPanning(bool panning) {
    MisPanning = panning;
    update();
}

bool GameScene::isPanning() const { return MisPanning; }

void GameScene::clearPathHighlight()
{
    for (HexItem* item : highlightedPath) {
        item->setPathHighlight(false);
    }
    highlightedPath.clear();
}

void GameScene::highlightPath()
{
    for (const QPoint& pos : currentPath) {
        const Hex& hex = Map.GetQPointLoc(pos);
        QPointF center = hex.GetCenter();

        QGraphicsItem* item = this->itemAt(center, QTransform());
        HexItem* hexItem = dynamic_cast<HexItem*>(item);

        if (hexItem) {
            hexItem->setPathHighlight(true);
            highlightedPath.push_back(hexItem);
        }
    }
}

void GameScene::StartMovement()
{
    if (isMoving) return;

    if (currentPath.empty()) {
        qDebug() << "Path not found or invalid target!";
        return;
    }

    isMoving = true;
    movementTimer->start(200);
}

void GameScene::processStep()
{
    if (currentPath.empty()) {
        movementTimer->stop();
        isMoving = false;
        return;
    }

    QPoint nextPos = currentPath.front();
    currentPath.erase(currentPath.begin());

    this->tryMoveHeroTo(nextPos);
    Map.UpdateVisibility(Hero.GetPosition());

    this->update();

    if (currentPath.empty()) {
        movementTimer->stop();
        isMoving = false;
        qDebug() << "Movement finished.";
    }
}
