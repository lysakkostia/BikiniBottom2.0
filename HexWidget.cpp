#include "HexWidget.h"
#include "Fight.h"
#include "GameConstants.h"
#include <QPainterPath>
#include <QMessageBox>

namespace Const_Scale = GlobalConst::TextureScale;

HexWidget::HexWidget(int NRadius, QWidget* parent) :
    QWidget(parent), Map(NRadius > 0 ? NRadius : 10), Hero(QPoint(0,0))
{
    setMinimumSize(800,600);
    setMouseTracking(true);
    InitializeTextures();
    Initialized = false;

    Map.UpdateVisibility(Hero.GetPosition());
}

QPixmap HexWidget::loadTexture(const QString &fileName, double scaleFactor)
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

void HexWidget::InitializeTextures()
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

QPixmap HexWidget::TintPixmap(const QPixmap& Source, qreal Strength)
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

QPixmap HexWidget::GetUnitTexture(UnitType type, bool isHeroOnHex)
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

void HexWidget::drawHexTerrain(QPainter& painter, const Hex& hex, const QPolygonF& polygon)
{
    QPixmap baseHexTexture;

    if (hex.VisibilityState()) {
        baseHexTexture = StandartVisibleHexTexture;
    } else if (hex.ExplorationState()) {
        baseHexTexture = StandartExploredHexTexture;
    } else {
        baseHexTexture = FogTexture;
    }

    QPainterPath hexClipPath;
    hexClipPath.addPolygon(polygon);

    painter.save();
    painter.setClipPath(hexClipPath);

    if (!baseHexTexture.isNull()) {
        QPointF center = hex.GetCenter();
        QPointF topLeft = center - QPointF(baseHexTexture.width() / 2.0, baseHexTexture.height() / 2.0);
        painter.drawPixmap(topLeft, baseHexTexture);
    } else {
        QColor color = Qt::black;
        if (hex.VisibilityState()) color = Qt::white;
        else if (hex.ExplorationState()) color = Qt::darkGray;

        painter.fillPath(hexClipPath, color);
    }
    painter.restore();
}

void HexWidget::drawUnitAndUI(QPainter& painter, const Hex& hex, const QPolygonF& polygon)
{
    if (!hex.VisibilityState() && !hex.ExplorationState()) return;

    QPixmap unitTexture;
    bool isHeroOnHex = (QPoint(hex.GetQR().first, hex.GetQR().second) == Hero.GetPosition());

    if (isHeroOnHex) {
        if (hex.HaveUnit()) {
            unitTexture = GetUnitTexture(hex.GetUnit()->GetType(), true);
        } else {
            unitTexture = HeroPixmap;
        }
    } else if (hex.HaveUnit()) {
        unitTexture = GetUnitTexture(hex.GetUnit()->GetType(), false);
    }

    if (unitTexture.isNull()) return;

    QPointF center = hex.GetCenter();
    QPointF topLeft = center - QPointF(unitTexture.width() / 2.0, unitTexture.height() / 2.0);

    QPainterPath hexClipPath;
    hexClipPath.addPolygon(polygon);

    painter.save();
    painter.setClipPath(hexClipPath);
    painter.drawPixmap(topLeft, unitTexture);

    if (!hex.VisibilityState() && hex.ExplorationState()) {
        painter.setCompositionMode(QPainter::CompositionMode_SourceAtop);
        painter.fillRect(QRectF(topLeft, unitTexture.size()), QColor(0, 0, 0, 100));
    }

    painter.restore();

    Unit* u = hex.GetUnit();
    if (u && u->IsEnemy() && (hex.VisibilityState() || hex.ExplorationState())) {
        drawLevelBadge(painter, center, u->GetLevel());
    }
}

void HexWidget::drawLevelBadge(QPainter& painter, const QPointF& center, int level)
{
    QString levelText = QString("Lvl %1").arg(level);

    QFont f = painter.font();
    f.setBold(true);
    f.setPointSize(14);
    painter.setFont(f);

    QPointF textPos(center.x() - 15, center.y() - Hex::HexSize / 2);

    painter.setPen(Qt::black);
    painter.drawText(textPos + QPointF(1, 1), levelText);

    painter.setPen(Qt::white);
    painter.drawText(textPos, levelText);
}

QPen HexWidget::getHexOutlinePen(const Hex& hex, const Hex& heroHex)
{
    QPoint currHexPoint(hex.GetQR().first, hex.GetQR().second);
    QPoint heroPoint(heroHex.GetQR().first, heroHex.GetQR().second);

    QPen pen(Qt::black, 1);

    if (currHexPoint == heroPoint) return pen;

    if (heroHex.IsNeighbor(hex)) {
        bool isBlocked = false;
        if (hex.HaveUnit()) {
            Unit* u = hex.GetUnit();
            if (u && u->GetType() == UnitType::StructUnBreak) {
                isBlocked = true;
            }
        }

        if (isBlocked) {
            pen.setColor(Qt::red);
            pen.setWidthF(2.0);
        } else {
            pen.setColor(QColor(255, 215, 0));
            pen.setWidthF(2.0);
        }
    }

    return pen;
}

void HexWidget::paintEvent(QPaintEvent*)
{
    QPainter Painter(this);
    Painter.setRenderHint(QPainter::Antialiasing);
    Painter.translate(OffsetX, OffsetY);
    Painter.scale(Scale, Scale);

    const auto& Grid = Map.GetMap();
    const Hex& HeroCurrHex = Map.GetQPointLoc(Hero.GetPosition());

    QVector<QPair<QPolygonF, QPen>> GoldHexes;
    QVector<QPair<QPolygonF, QPen>> RedHexes;

    for (const auto& Col : Grid)
    {
        for (const auto& Hex_ : Col)
        {
            QPolygonF Polygon;
            for (const auto& c : Hex_.GetCorners()) {
                Polygon << c;
            }

            drawHexTerrain(Painter, Hex_, Polygon);
            drawUnitAndUI(Painter, Hex_, Polygon);

            QPoint currentHexPos(Hex_.GetQR().first, Hex_.GetQR().second);

            if (currentHexPos == HoveredHex)
            {
                if (Hex_.VisibilityState() || Hex_.ExplorationState())
                {
                    Painter.save();
                    Painter.setPen(Qt::NoPen);
                    Painter.setBrush(QColor(255, 255, 255, 60));
                    Painter.drawPolygon(Polygon);
                    Painter.restore();
                }
            }

            QPen OutlinePen = getHexOutlinePen(Hex_, HeroCurrHex);

            if (OutlinePen.color() == Qt::black) {
                Painter.setBrush(Qt::NoBrush);
                Painter.setPen(OutlinePen);
                Painter.drawPolygon(Polygon);
            }
            else if (OutlinePen.color() == Qt::red) {
                RedHexes.append({Polygon, OutlinePen});
            }
            else {
                GoldHexes.append({Polygon, OutlinePen});
            }
        }
    }

    Painter.setBrush(Qt::NoBrush);
    for (const auto& item : GoldHexes) {
        Painter.setPen(item.second);
        Painter.drawPolygon(item.first);
    }

    for (const auto& item : RedHexes) {
        Painter.setPen(item.second);
        Painter.drawPolygon(item.first);
    }
}

void HexWidget::wheelEvent(QWheelEvent* event)
{
    float Factor = 1.1;
    float Zoom = (event->angleDelta().y() > 0) ? Factor : 1.0 / Factor;

    QPointF CursorPos = event->position();
    QPointF BeforeScale = (CursorPos - QPointF(OffsetX, OffsetY)) / Scale;

    Scale *= Zoom;

    QPointF AfterScale = (CursorPos - QPointF(OffsetX, OffsetY)) / Scale;

    QPointF OffsetDiff = (AfterScale - BeforeScale) * Scale;
    OffsetX += OffsetDiff.x();
    OffsetY += OffsetDiff.y();

    update();
}

void HexWidget::handleLeftClick(const QPointF& pos)
{
    QPointF Cord = (pos - QPointF(OffsetX, OffsetY)) / Scale;
    QPoint HexCord = PixelToHex(Cord);

    if (Map.ContainsHex(HexCord.x(), HexCord.y()))
    {
        if (tryMoveHeroTo(HexCord))
        {
            Map.UpdateVisibility(Hero.GetPosition());
            update();
            emit heroStatsChanged();
        }
    }
}

bool HexWidget::tryMoveHeroTo(const QPoint& targetHexCoords)
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

    return true;
}

void HexWidget::interactWithContentOnHex(const Hex& hex, const QPoint& previousPos)
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

void HexWidget::processCombat(Unit* enemy, const QPoint& previousPos)
{
    qWarning("Hero moved onto an enemy hex! Starting fight.");

    QPixmap enemyDisplayTexture = getEnemyTexture(enemy->GetType());

    Fight fightDialog(enemyDisplayTexture, &Hero, enemy, this);
    int fightResultCode = fightDialog.exec();

    bool playerEscaped = fightDialog.didPlayerEscaped();

    if (fightResultCode == QDialog::Accepted)
    {
        qDebug("Fight won!");
        Map.ClearUnitAt(Hero.GetPosition());
        Hero.LevelUp();
        Map.DecrementEnemyCount();

        if (Map.GetEnemyCount() <= 0) {
            QMessageBox::information(this, tr("Victory!"), tr("Congratulations! You have defeated all enemies!"));
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

QPixmap HexWidget::getEnemyTexture(UnitType type)
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

void HexWidget::processFriendly(Unit* friendUnit)
{
    if (friendUnit->GetAI()) {
        Friendly* friendlyAI = dynamic_cast<Friendly*>(friendUnit->GetAI());
        if (friendlyAI) {
            QMessageBox::information(this, tr("Friendly NPC"), QString::fromStdString(friendlyAI->getGreeting()));
        }
    }
}

void HexWidget::processCampfire(Unit* campfireUnit)
{
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

    QMessageBox::information(this, tr("Campfire"), msg);

    if (campfire->GetHP() <= 0) {
        QMessageBox::information(this, tr("Campfire"), tr("The campfire has extinguished."));
        Map.ClearUnitAt(Hero.GetPosition());
    }
}

void HexWidget::processTreasure(Unit* treasureUnit)
{
    QMessageBox::information(this, tr("Treasure Chest"), tr("You found a chest! Level Up!"));
    Hero.LevelUp();
    Map.ClearUnitAt(Hero.GetPosition());
}

void HexWidget::mousePressEvent(QMouseEvent* event)
{
    if (event->button() == Qt::RightButton)
    {
        IsDragging = true;
        LastMousePos = event->pos();
    }
    else if (event->button() == Qt::LeftButton)
    {
        handleLeftClick(event->pos());
    }
}

void HexWidget::mouseMoveEvent(QMouseEvent* event)
{
    if(IsDragging)
    {
        QPointF Delta = event->pos() - LastMousePos;
        OffsetX += Delta.x();
        OffsetY += Delta.y();
        LastMousePos = event->pos();
        update();
    }
    else
    {
        QPointF Pos = event->pos();
        QPointF Cord = (Pos - QPointF(OffsetX, OffsetY)) / Scale;
        QPoint HexCord = PixelToHex(Cord);

        if(Map.ContainsHex(HexCord.x(),HexCord.y()))
        {
            if(HexCord != HoveredHex)
            {
                HoveredHex = HexCord;
                update();
            }
        }
        else if(HoveredHex != QPoint(INT_MAX,INT_MAX))
        {
            HoveredHex = QPoint(INT_MAX,INT_MAX);
            update();
        }
    }
}

void HexWidget::mouseReleaseEvent(QMouseEvent* event)
{
    if(event->button() == Qt::RightButton)
        IsDragging = false;
}

void HexWidget::resizeEvent(QResizeEvent* event)
{
    QWidget::resizeEvent(event);

    if(!Initialized)
    {
        QRectF MapRect = GetMapBoundingRect();
        if(!MapRect.isNull() && MapRect.isValid())
        {
            QPointF MapCenter = MapRect.center();
            QPointF WidgetCenter = QPointF(width(), height()) / 2.0;

            OffsetX = WidgetCenter.x() - MapCenter.x() * Scale;
            OffsetY = WidgetCenter.y() - MapCenter.y() * Scale;

            Initialized = true;
            update();
        }
    }
}

void HexWidget::leaveEvent(QEvent*)
{
    if(HoveredHex != QPoint(INT_MAX,INT_MAX))
    {
        HoveredHex = QPoint(INT_MAX,INT_MAX);
        update();
    }
}

QPoint HexWidget::PixelToHex(QPointF p) const
{
    float q = (2.0 / 3.0 * p.x()) / Hex::HexSize;
    float r = (-1.0 / 3.0 * p.x() + std::sqrt(3.0) / 3.0 * p.y()) / Hex::HexSize;
    return CubeToAxial(q,r);
}

QPoint HexWidget::CubeToAxial(float qc, float rc) const
{
    float xc = qc;
    float zc = rc;
    float yc = -xc - zc;

    int xa = std::round(xc);
    int ya = std::round(yc);
    int za = std::round(zc);

    float dx = std::abs(xc - xa);
    float dy = std::abs(yc - ya);
    float dz = std::abs(zc - za);

    if(dx > dy && dx > dz)
        xa = -ya - za;
    else if(dy > dz)
        ya = -xa - za;
    else
        za = -xa - ya;

    return QPoint(xa,za);
}

QRectF HexWidget::GetMapBoundingRect() const
{
    const auto& Grid = Map.GetMap();
    if(Grid.empty()) return QRectF();

    qreal MinX = std::numeric_limits<float>::max();
    qreal MaxX = std::numeric_limits<float>::lowest();
    qreal MinY = std::numeric_limits<float>::max();
    qreal MaxY = std::numeric_limits<float>::lowest();

    for(const auto& Col : Grid)
    {
        for(const auto& Hex_ : Col)
        {
            QPointF Center = Hex_.GetCenter();
            MinX = std::min(MinX, Center.x());
            MaxX = std::max(MaxX, Center.x());
            MinY = std::min(MinY, Center.y());
            MaxY = std::max(MaxY, Center.y());
        }
    }

    return QRectF(MinX - Hex::HexSize,
                  MinY - Hex::HexSize,
                  (MaxX - MinX) + 2 * Hex::HexSize,
                  (MaxY - MinY) + 2 * Hex::HexSize);
}

void HexWidget::SaveMapToFile(const QString& filePath)
{
    HeroStats CurrentStats = GetStats();
    Map.SaveToFile(filePath, Hero.GetPosition(), CurrentStats.HP, CurrentStats.MP, CurrentStats.LVL);
}

bool HexWidget::LoadMapFromFile(const QString& filePath)
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
        Initialized = false;
        update();
        emit heroStatsChanged();
    }
    return Success;
}

HexWidget::HeroStats HexWidget::GetStats()
{
    HeroStats stats;
    stats.HP = Hero.GetHP();
    stats.MP = Hero.GetMana();
    stats.LVL = Hero.GetLevel();
    return stats;
}
