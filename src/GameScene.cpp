#include "GameScene.h"
#include "GameConstants.h"
#include "Fight.h"
#include "AI.h"
#include "TextureManager.h"
#include "CampfireWidget.h"
#include <QMessageBox>
#include <QGraphicsView>
#include <QDebug>
#include <QEventLoop>

namespace Const_Scale = GlobalConst::TextureScale;

GameScene::GameScene(int NRadius, unsigned int seed, QObject *parent)
    : QGraphicsScene(parent), mapInner(NRadius > 0 ? NRadius : 10, seed), heroInner(QPoint(0,0))
{
    mapInner.updateVisibility(heroInner.getPosition());
    generateMapItems();
    movementTimer = new QTimer(this);
    connect(movementTimer, &QTimer::timeout, this, &GameScene::processStep);
    isMoving = false;
}

void GameScene::generateMapItems()
{
    this->clear();

    const auto& Grid = mapInner.getMap();
    for (const auto& Col : Grid)
    {
        for (const auto& Hex_ : Col)
        {
            Hex* modelHex = const_cast<Hex*>(&Hex_);

            HexItem* item = new HexItem(modelHex, this);
            this->addItem(item);
            hexItemsMap.insert(QPoint(modelHex->getQR().first, modelHex->getQR().second), item);
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

bool GameScene::tryMoveHeroTo(const QPoint& targetHexCoords, bool npc_interaction)
{
    QPoint currentPos = heroInner.getPosition();
    const Hex& currentHex = mapInner.getQPointLoc(currentPos);
    const Hex& targetHex = mapInner.getQPointLoc(targetHexCoords);

    if (!currentHex.isNeighbor(targetHex)) return false;

    if (targetHex.haveUnit()) {
        Unit* u = targetHex.getUnit();
        if (u && u->getType() == UnitType::StructUnBreak) return false;
    }

    heroInner.setPosition(targetHexCoords);

    const Hex& newHexLocation = mapInner.getQPointLoc(heroInner.getPosition());
    if (newHexLocation.haveUnit() && npc_interaction == true) {
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
    Unit* unit = hex.getUnit();
    if (!unit) return;

    if (unit->isEnemy())
    {
        processCombat(unit, previousPos);
    }
    else if (unit->getType() == UnitType::Friend)
    {
        processFriendly(unit);
    }
    else if (unit->getType() == UnitType::CampfireUnit)
    {
        processCampfire(unit);
    }
    else if (unit->getType() == UnitType::StructBreak)
    {
        processTreasure(unit);
    }
}

void GameScene::processCombat(Unit* enemy, const QPoint& previousPos)
{
    qWarning("Hero moved onto an enemy hex! Requesting combat UI.");
    lastPreCombatPos = previousPos;
    emit combatStarted();
    emit combatRequested(enemy);
}

void GameScene::finishCombat(bool playerWon, bool playerEscaped, Unit* enemy)
{
    emit combatEnded();

    if (playerWon)
    {
        qDebug("Fight won!");

        double xpReward = GlobalConst::Progression::ENEMY_XP_REWARD * enemy->getLevel();
        if (enemy->getType() == UnitType::Wizard || enemy->getType() == UnitType::Barbarian) {
            xpReward *= 1.2;
        }

        heroInner.addXP(xpReward);
        emit heroStatsChanged();

        qDebug() << "Gained XP:" << xpReward;

        mapInner.clearUnitAt(heroInner.getPosition());

        if (heroInner.isLevelUpPending()) {
            emit levelUpTriggered();
        }
        if (mapInner.getEnemyCount() <= 0) {
            emit victory();
        }
    }
    else
    {
        if (heroInner.getHP() <= 0) {
            qDebug("Fight lost. Game Over.");
            emit gameOver();
        }
        else {
            qDebug() << (playerEscaped ? "Hero escaped" : "Dialog closed unexpectedly");
            this->tryMoveHeroTo(lastPreCombatPos);
            mapInner.updateVisibility(heroInner.getPosition());
            emit heroStatsChanged();
        }
    }

    QWidget* view = getViewWidget();
    if (view) view->setFocus();
}

void GameScene::processCampfire(Unit* campfireUnit)
{
    CampfireUnit* campfire = dynamic_cast<CampfireUnit*>(campfireUnit);
    if (!campfire || !campfire->getAI()) return;

    Campfire* campfireAI = dynamic_cast<Campfire*>(campfire->getAI());
    if (!campfireAI) return;

    double oldHP = heroInner.getHP();
    double oldMana = heroInner.getMana();

    campfireAI->heal(&heroInner);

    float currentCharges = campfire->getHP();
    campfire->setHP(currentCharges - 1);

    int remainingCharges = static_cast<int>(campfire->getHP());

    emit campfireRequested(oldHP, heroInner.getHP(), oldMana, heroInner.getMana(), remainingCharges, campfireUnit);
}

void GameScene::finishCampfireInteraction(Unit* campfireUnit)
{
    CampfireUnit* campfire = dynamic_cast<CampfireUnit*>(campfireUnit);
    if (campfire && campfire->getHP() <= 0) {
        mapInner.clearUnitAt(heroInner.getPosition());
        this->update();
    }

    emit heroStatsChanged();

    QWidget* view = getViewWidget();
    if (view) view->setFocus();
}

void GameScene::processFriendly(Unit* friendUnit)
{
    if (friendUnit->getAI()) {
        Friendly* friendlyAI = dynamic_cast<Friendly*>(friendUnit->getAI());
        if (friendlyAI) {
            QString greeting = QString::fromStdString(friendlyAI->getGreeting());
            emit npcInteractionRequested(friendUnit, greeting);
        }
    }
}

void GameScene::finishNPCInteraction()
{
    QWidget* view = getViewWidget();
    if (view) view->setFocus();
}

void GameScene::processTreasure(Unit* treasureUnit)
{
    double neededXP = heroInner.getMaxXP() - heroInner.getCurrentXP();
    if (neededXP > 0) {
        heroInner.addXP(neededXP);
    } else {
        heroInner.addXP(1);
    }

    emit heroStatsChanged();

    mapInner.clearUnitAt(heroInner.getPosition());
    if (heroInner.isLevelUpPending()) {
        emit levelUpTriggered();
    }
}

void GameScene::handleHexClick(HexItem* item)
{
    if (isHeroMoving() || checkPaused()) return;

    Hex* targetHex = item->getModelHex();
    QPoint targetPos(targetHex->getQR().first, targetHex->getQR().second);

    if (tryMoveHeroTo(targetPos))
    {
        mapInner.updateVisibility(heroInner.getPosition());

        this->update();
        emit heroStatsChanged();
    }

    if (targetPos == pendingTarget)
    {
        startMovement();
        clearPathHighlight();
        pendingTarget = QPoint(-999, -999);
    }
    else
    {
        clearPathHighlight();

        currentPath = mapInner.findPath(heroInner.getPosition(), targetPos);

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
    return const_cast<Hex*>(&mapInner.getQPointLoc(heroInner.getPosition()));
}

void GameScene::saveMapToFile()
{
    if (currentSaveFilePath.isEmpty()) {
        qDebug() << "Error: No save file path set!";
        return;
    }
    mapInner.saveToFile(currentSaveFilePath, heroInner);
}

bool GameScene::loadMapFromFile(const QString& filePath)
{
    bool Success = mapInner.loadFromFile(filePath, heroInner);

    if(Success)
    {
        mapInner.updateVisibility(heroInner.getPosition());
        generateMapItems();
        emit heroStatsChanged();
    }
    return Success;
}

GameScene::HeroStats GameScene::getStats()
{
    HeroStats stats;
    stats.hp = heroInner.getHP();
    stats.mp = heroInner.getMana();
    stats.lvl = heroInner.getLevel();
    return stats;
}

void GameScene::setPanning(bool panning) {
    isPanning = panning;
    update();
}

bool GameScene::checkPanning() const { return isPanning; }

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
        if (hexItemsMap.contains(pos)) {
            HexItem* hexItem = hexItemsMap.value(pos);
            hexItem->setPathHighlight(true);
            highlightedPath.push_back(hexItem);
        }
    }
}

void GameScene::startMovement()
{
    if (isMoving) return;

    if (currentPath.empty()) {
        qDebug() << "Path not found or invalid target!";
        return;
    }

    currentPathIndex = 0;
    isMoving = true;
    movementTimer->start(200);
}

void GameScene::processStep()
{
    if (currentPathIndex >= currentPath.size()) {
        movementTimer->stop();
        isMoving = false;
        return;
    }

    QPoint nextPos = currentPath[currentPathIndex];
    currentPathIndex++;

    this->tryMoveHeroTo(nextPos, false);
    mapInner.updateVisibility(heroInner.getPosition());

    this->update();

    if (currentPathIndex >= currentPath.size()) {
        movementTimer->stop();
        isMoving = false;
        currentPath.clear();
        qDebug() << "Movement finished.";
    }
}

void GameScene::setPaused(bool paused) {
    isPaused = paused;
    if (paused) {
        movementTimer->stop();
    } else if (isMoving) {
        movementTimer->start(200);
    }
}
