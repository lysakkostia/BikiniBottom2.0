#include "GameScene.h"
#include "GameConstants.h"
#include "Fight.h"
#include "AI.h"
#include <QMessageBox>
#include <QGraphicsView>
#include <QDebug>

namespace Const_Scale = GlobalConst::TextureScale;

GameScene::GameScene(int NRadius, QObject *parent)
    : QGraphicsScene(parent), Map(NRadius > 0 ? NRadius : 10), Hero(QPoint(0,0))
{
    InitializeTextures();
    Map.UpdateVisibility(Hero.GetPosition());
    generateMapItems();
}

QPixmap GameScene::loadTexture(const QString &fileName, double scaleFactor)
{
    QPixmap originalPixmap(fileName);

    if (originalPixmap.isNull()) {
        qWarning() << "Failed to load texture:" << fileName;
        return QPixmap();
    }

    return originalPixmap.scaled(
        QSizeF(scaleFactor * Hex::HexSize, scaleFactor * Hex::HexSize).toSize(),
        Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void GameScene::InitializeTextures()
{
    this->HeroPixmap = loadTexture("NPC5Texture.png", Const_Scale::HERO);
    this->FogTexture = loadTexture("FogTexture.png", Const_Scale::STANDART_HEX);
    this->BarbarianTexture = loadTexture("NPC4Texture.png", Const_Scale::UNIT);
    this->WarriorTexture = loadTexture("NPC1Texture.png", Const_Scale::UNIT);
    this->WizardTexture = loadTexture("NPC7Texture.png", Const_Scale::UNIT);
    this->FriendTexture = loadTexture("NPC2Texture.png", Const_Scale::UNIT);
    this->StructBreakTexture = loadTexture("NPC3Texture.png", Const_Scale::UNIT);
    this->StructUnBreakTexture = loadTexture("MountainTexture.png", Const_Scale::MOUNTAIN);
    this->CampfireTexture = loadTexture("NPC6Texture.png", Const_Scale::UNIT);
    this->HeroWithWarriorTexture = loadTexture("HeroWithEnemyTexture.png", Const_Scale::UNIT);
    this->HeroWithBarbarianTexture = loadTexture("HeroWithCocosikTexture.png", Const_Scale::UNIT);
    this->HeroWithWizardTexture = loadTexture("HeroWithWizardTexture.png", Const_Scale::UNIT);
    this->HeroWithFriendTexture = loadTexture("HeroWithFriendTexture.png", Const_Scale::UNIT);
    this->HeroWithStructTexture = loadTexture("HeroWithStructTexture.png", Const_Scale::UNIT);
    this->HeroWithCampfireTexture = loadTexture("HeroWithCampfireTexture.png", Const_Scale::UNIT);

    this->StandartVisibleHexTexture = loadTexture("StandartHex.jpg", Const_Scale::STANDART_HEX);
    if (!this->StandartVisibleHexTexture.isNull()) {
        this->StandartExploredHexTexture = TintPixmap(this->StandartVisibleHexTexture, 0.4);
    }
}

QPixmap GameScene::TintPixmap(const QPixmap& Source, qreal Strength)
{
    if(Source.isNull())
        return Source;

    QPixmap TintedPixmap = Source;
    QPainter p(&TintedPixmap);
    QColor OverlayColor = Qt::black;
    OverlayColor.setAlphaF(Strength);
    p.fillRect(TintedPixmap.rect(), OverlayColor);
    p.end();
    return TintedPixmap;
}

QPixmap GameScene::getEnemyTexture(UnitType type)
{
    QPixmap texture;
    switch (type) {
    case UnitType::Barbarian: texture = this->BarbarianTexture; break;
    case UnitType::Warrior:   texture = this->WarriorTexture; break;
    case UnitType::Wizard:    texture = this->WizardTexture; break;
    default: break;
    }

    if (texture.isNull()) {
        texture = QPixmap(200, 150);
        texture.fill(Qt::red);
    }
    return texture;
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
            hexItem->update();
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

    Fight fightDialog(getEnemyTexture(enemy->GetType()), &Hero, enemy, parentView);
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

QPixmap GameScene::getTerrainTexture(bool visible, bool explored)
{
    if (visible) return StandartVisibleHexTexture;
    if (explored) return StandartExploredHexTexture;
    return FogTexture;
}

QPixmap GameScene::getUnitTexture(UnitType type, bool isHeroOnHex)
{
    if (isHeroOnHex) {
        switch (type) {
        case UnitType::Barbarian: return HeroWithBarbarianTexture;
        case UnitType::Warrior:   return HeroWithWarriorTexture;
        case UnitType::Wizard:    return HeroWithWizardTexture;
        case UnitType::Friend:    return HeroWithFriendTexture;
        case UnitType::StructBreak: return HeroWithStructTexture;
        case UnitType::CampfireUnit:  return HeroWithCampfireTexture;
        default: return HeroPixmap;
        }
    } else {
        switch (type) {
        case UnitType::Barbarian: return BarbarianTexture;
        case UnitType::Warrior:   return WarriorTexture;
        case UnitType::Wizard:    return WizardTexture;
        case UnitType::Friend:    return FriendTexture;
        case UnitType::StructBreak: return StructBreakTexture;
        case UnitType::StructUnBreak: return StructUnBreakTexture;
        case UnitType::CampfireUnit:  return CampfireTexture;
        case UnitType::MainHero: return HeroPixmap;
        default: return QPixmap();
        }
    }
}

void GameScene::handleHexClick(HexItem* item)
{
    Hex* targetHex = item->getModelHex();
    QPoint targetPos(targetHex->GetQR().first, targetHex->GetQR().second);

    if (tryMoveHeroTo(targetPos))
    {
        Map.UpdateVisibility(Hero.GetPosition());

        this->update();
        emit heroStatsChanged();
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
