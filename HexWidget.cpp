#include "HexWidget.h"
#include "Fight.h"
#include <QPainterPath>
#include <QMessageBox>

HexWidget::HexWidget(int NRadius, QWidget* parent) :
    QWidget(parent), Map(NRadius > 0 ? NRadius : 10), Hero(QPoint(0,0))
{
    setMinimumSize(800,600);
    setMouseTracking(true);
    InitializeTextures();
    Initialized = false;

    Map.UpdateVisibility(Hero.GetPosition());
}

void HexWidget::InitializeTextures()
{
    QPixmap HeroOriginalPixmap("NPC5Texture.png");
    if(!HeroOriginalPixmap.isNull())
    {
        this->HeroPixmap = HeroOriginalPixmap.scaled(
            QSizeF(2 * Hex::HexSize, 2 * Hex::HexSize).toSize(),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else
    {
        qWarning("Failed to load texture");
    }

    QPixmap FogOriginalPixmap("FogTexture.png");
    if(!FogOriginalPixmap.isNull())
    {
        this->FogTexture = FogOriginalPixmap.scaled(
            QSizeF(2 * Hex::HexSize, 2 * Hex::HexSize).toSize(),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else
    {
        qWarning("Failed to load texture");
    }

    QPixmap BarbarianOriginalPixmap("NPC4Texture.png");
    if(!BarbarianOriginalPixmap.isNull())
    {
        this->BarbarianTexture = BarbarianOriginalPixmap.scaled(
            QSizeF(1.7 * Hex::HexSize, 1.7 * Hex::HexSize).toSize(),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else
    {
        qWarning("Failed to load texture");
    }

    QPixmap WarriorOriginalPixmap("NPC1Texture.png");
    if(!WarriorOriginalPixmap.isNull())
    {
        this->WarriorTexture = WarriorOriginalPixmap.scaled(
            QSizeF(1.7 * Hex::HexSize, 1.7 * Hex::HexSize).toSize(),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else
    {
        qWarning("Failed to load texture");
    }

    QPixmap WizardOriginalPixmap("NPC7Texture.png");
    if(!WizardOriginalPixmap.isNull())
    {
        this->WizardTexture = WizardOriginalPixmap.scaled(
            QSizeF(1.7 * Hex::HexSize, 1.7 * Hex::HexSize).toSize(),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else
    {
        qWarning("Failed to load texture");
    }

    QPixmap FriendOriginalPixmap("NPC2Texture.png");
    if(!FriendOriginalPixmap.isNull())
    {
        this->FriendTexture = FriendOriginalPixmap.scaled(
            QSizeF(1.7 * Hex::HexSize, 1.7 * Hex::HexSize).toSize(),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else
    {
        qWarning("Failed to load texture");
    }

    QPixmap StructBreakOriginalPixmap("NPC3Texture.png");
    if(!StructBreakOriginalPixmap.isNull())
    {
        this->StructBreakTexture = StructBreakOriginalPixmap.scaled(
            QSizeF(1.7 * Hex::HexSize, 1.7 * Hex::HexSize).toSize(),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else
    {
        qWarning("Failed to load texture");
    }

    QPixmap StructUnBreakOriginalPixmap("MountainTexture.png");
    if(!StructUnBreakOriginalPixmap.isNull())
    {
        this->StructUnBreakTexture = StructUnBreakOriginalPixmap.scaled(
            QSizeF(1.9 * Hex::HexSize, 1.9 * Hex::HexSize).toSize(),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else
    {
        qWarning("Failed to load texture");
    }

    QPixmap CampfireOriginalPixmap("NPC6Texture.png");
    if(!CampfireOriginalPixmap.isNull())
    {
         this->CampfireTexture = CampfireOriginalPixmap.scaled(
            QSizeF(1.7 * Hex::HexSize, 1.7 * Hex::HexSize).toSize(),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else
    {
        qWarning("Failed to load texture");
    }

    QPixmap StandartHexOriginalPixmap("StandartHex.jpg");
    if(!StandartHexOriginalPixmap.isNull())
    {
        this->StandartVisibleHexTexture = StandartHexOriginalPixmap.scaled(
            QSizeF(2 * Hex::HexSize, 2 * Hex::HexSize).toSize(),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);

        this->StandartExploredHexTexture = TintPixmap(this->StandartVisibleHexTexture, 0.4);
    }
    else
    {
        qWarning("Failed to load texture");
    }

    QPixmap HeroWithWarrioryOriginalPixmap("HeroWithEnemyTexture.png");
    if(!HeroWithWarrioryOriginalPixmap.isNull())
    {
        this->HeroWithWarriorTexture = HeroWithWarrioryOriginalPixmap.scaled(
            QSizeF(1.7 * Hex::HexSize, 1.7 * Hex::HexSize).toSize(),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);



    }
    else
    {
        qWarning("Failed to load texture");
    }

    QPixmap HeroWithBarbarianOriginalPixmap("HeroWithCocosikTexture.png");
    if(!HeroWithBarbarianOriginalPixmap.isNull())
    {
        this->HeroWithBarbarianTexture = HeroWithBarbarianOriginalPixmap.scaled(
            QSizeF(1.7 * Hex::HexSize, 1.7 * Hex::HexSize).toSize(),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);



    }
    else
    {
        qWarning("Failed to load texture");
    }

    QPixmap HeroWithWizardOriginalPixmap("HeroWithWizardTexture.png");
    if(!HeroWithWizardOriginalPixmap.isNull())
    {
        this->HeroWithWizardTexture = HeroWithWizardOriginalPixmap.scaled(
            QSizeF(1.7 * Hex::HexSize, 1.7 * Hex::HexSize).toSize(),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);



    }
    else
    {
        qWarning("Failed to load texture");
    }

    QPixmap HeroWithFriendOriginalPixmap("HeroWithFriendTexture.png");
    if(!HeroWithFriendOriginalPixmap.isNull())
    {
        this->HeroWithFriendTexture = HeroWithFriendOriginalPixmap.scaled(
            QSizeF(1.7 * Hex::HexSize, 1.7 * Hex::HexSize).toSize(),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else
    {
        qWarning("Failed to load texture");
    }

    QPixmap HeroWithStructOriginalPixmap("HeroWithStructTexture.png");
    if(!HeroWithStructOriginalPixmap.isNull())
    {
        this->HeroWithStructTexture = HeroWithStructOriginalPixmap.scaled(
            QSizeF(1.7 * Hex::HexSize, 1.7 * Hex::HexSize).toSize(),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else
    {
        qWarning("Failed to load texture");
    }

    QPixmap HeroWithCampfireOriginalPixmap("HeroWithCampfireTexture.png");
    if(!HeroWithCampfireOriginalPixmap.isNull())
    {
        this->HeroWithCampfireTexture = HeroWithCampfireOriginalPixmap.scaled(
            QSizeF(1.7 * Hex::HexSize, 1.7 * Hex::HexSize).toSize(),
            Qt::KeepAspectRatio, Qt::SmoothTransformation);
    }
    else
    {
        qWarning("Failed to load texture");
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

void HexWidget::paintEvent(QPaintEvent*)
{
    QPainter Painter(this);
    Painter.setRenderHint(QPainter::Antialiasing);

    Painter.translate(OffsetX, OffsetY);
    Painter.scale(Scale, Scale);

    const auto& Grid = Map.GetMap();
    const Hex& HeroCurrHex = Map.GetQPointLoc(Hero.GetPosition());

    QHash<QPoint, QPen> HexOutlines;
    QHash<QPoint, QPolygonF> HexPolygons;

    for(const auto& Col : Grid)
    {
        for(const auto& Hex_ : Col)
        {
            QPoint CurrHexQPoint(Hex_.GetQR().first, Hex_.GetQR().second);
            auto Corners = Hex_.GetCorners();

            QPolygonF Polygon;
            for(const auto& c : Corners)
                Polygon << c;

            HexPolygons[CurrHexQPoint] = Polygon;

            QPainterPath HexClipPath;
            HexClipPath.addPolygon(Polygon);

            QPixmap BaseHexTexture;
            bool IsHexVisible = Hex_.VisibilityState();
            bool IsHexExplored = Hex_.ExplorationState();
            QPointF center = Hex_.GetCenter();
            Unit* CurrentUnit = NULL;
            if(IsHexVisible)
                BaseHexTexture = StandartVisibleHexTexture;
            else if(IsHexExplored)
                BaseHexTexture = StandartExploredHexTexture;
            else
                BaseHexTexture = FogTexture;

            if(!BaseHexTexture.isNull())
            {
                QPointF topLeft = center - QPointF(BaseHexTexture.width() / 2.0, BaseHexTexture.height() / 2.0);

                Painter.save();
                Painter.setClipPath(HexClipPath);
                Painter.drawPixmap(topLeft, BaseHexTexture);
                Painter.restore();
            }
            else
            {
                QBrush Brush;
                if(IsHexVisible)
                    Brush = Qt::white;
                else if(IsHexExplored)
                    Brush = Qt::darkGray;
                else
                    Brush = Qt::black;

                Painter.save();
                Painter.setClipPath(HexClipPath);
                Painter.fillPath(HexClipPath, Brush);
                Painter.restore();
            }

            if(IsHexVisible || IsHexExplored)
            {
                QPixmap UnitTexture;
                bool IsHeroOnHex = (QPoint(Hex_.GetQR().first, Hex_.GetQR().second) == Hero.GetPosition());

                if (IsHeroOnHex) {
                    // Якщо герой тут, перевіряємо чи є під ним інший юніт (наприклад, багаття)
                    if (Hex_.HaveUnit()) {
                        UnitTexture = GetUnitTexture(Hex_.GetUnit()->GetType(), true);
                    } else {
                        UnitTexture = HeroPixmap;
                    }
                } else if (Hex_.HaveUnit()) {
                    UnitTexture = GetUnitTexture(Hex_.GetUnit()->GetType(), false);
                }

                if(!UnitTexture.isNull())
                {
                    QPixmap FinalUnitTexture = UnitTexture;

                    if(!IsHexVisible && IsHexExplored)
                        FinalUnitTexture = TintPixmap(UnitTexture, 0.4);

                    QPointF center = Hex_.GetCenter();
                    QPointF topLeft = center - QPointF(FinalUnitTexture.width() / 2.0, FinalUnitTexture.height() / 2.0);

                    Painter.save();
                    Painter.setClipPath(HexClipPath);
                    Painter.drawPixmap(topLeft, FinalUnitTexture);
                    Painter.restore();
                }
                if (Hex_.HaveUnit()) {
                    Unit* u = Hex_.GetUnit();
                    // Переконуємось, що це ворог і гекс видимий (або розвіданий, якщо хочемо бачити "пам'ять" про рівень)
                    if (u && u->IsEnemy() && (Hex_.VisibilityState() || Hex_.ExplorationState())) {

                        QString levelText = QString("Lvl %1").arg(u->GetLevel());

                        // Налаштування шрифту
                        QFont f = Painter.font();
                        f.setBold(true);
                        f.setPointSize(14); // Трохи менший шрифт, щоб влазив
                        Painter.setFont(f);

                        // Координати тексту (над головою юніта)
                        QPointF textPos(center.x() - 15, center.y() - Hex::HexSize / 2);

                        // Малюємо чорну підкладку (тінь) для читабельності
                        Painter.setPen(Qt::black);
                        Painter.drawText(textPos + QPointF(1, 1), levelText);

                        // Малюємо основний текст (червоний для ворогів)
                        Painter.setPen(QColor(255, 255, 255)); // Яскраво-червоний
                        Painter.drawText(textPos, levelText);
                    }
                }
            }

            QPen OutlinePen(Qt::black, 1);

            if(CurrHexQPoint != Hero.GetPosition())
            {
                bool IsNeighborToHero = HeroCurrHex.IsNeighbor(Hex_);

                if(IsNeighborToHero)
                {
                    bool IsBlocked = false;
                    if(Hex_.HaveUnit())
                    {
                        Unit* Unit_ = Hex_.GetUnit();
                        if(Unit_ && Unit_->GetType() == UnitType::StructUnBreak)
                        {
                            IsBlocked = true;
                        }
                    }

                    if(IsBlocked)
                    {
                        OutlinePen.setColor(Qt::red);
                        OutlinePen.setWidthF(2.0);
                    }
                    else
                    {
                        OutlinePen.setColor(QColor(255, 215, 0));
                        OutlinePen.setWidthF(2.0);
                    }
                }
            }
            HexOutlines[CurrHexQPoint] = OutlinePen;
        }
    }

    QHashIterator<QPoint, QPolygonF> IterPolygons(HexPolygons);
    while(IterPolygons.hasNext())
    {
        IterPolygons.next();
        const QPoint& CurrHexQPoint = IterPolygons.key();
        const QPolygonF& Polygon = IterPolygons.value();

        if(HexOutlines.contains(CurrHexQPoint))
        {
            const QPen& CurrPen = HexOutlines[CurrHexQPoint];
            if(CurrPen.color() == Qt::black)
            {
                Painter.setBrush(Qt::NoBrush);
                Painter.setPen(CurrPen);
                Painter.drawPolygon(Polygon);
            }
        }
    }

    QHashIterator<QPoint, QPolygonF> IterPolygonsGold(HexPolygons);
    while(IterPolygonsGold.hasNext())
    {
        IterPolygonsGold.next();
        const QPoint& CurrHexQPoint = IterPolygonsGold.key();
        const QPolygonF& Polygon = IterPolygonsGold.value();

        if(HexOutlines.contains(CurrHexQPoint))
        {
            const QPen& CurrPen = HexOutlines[CurrHexQPoint];
            if(CurrPen.color() == QColor(255, 215, 0))
            {
                Painter.setBrush(Qt::NoBrush);
                Painter.setPen(CurrPen);
                Painter.drawPolygon(Polygon);
            }
        }
    }

    QHashIterator<QPoint, QPolygonF> IterPolygonsRed(HexPolygons);
    while(IterPolygonsRed.hasNext())
    {
        IterPolygonsRed.next();
        const QPoint& CurrHexQPoint = IterPolygonsRed.key();
        const QPolygonF& Polygon = IterPolygonsRed.value();

        if(HexOutlines.contains(CurrHexQPoint))
        {
            const QPen& CurrPen = HexOutlines[CurrHexQPoint];
            if(CurrPen.color() == Qt::red)
            {
                Painter.setBrush(Qt::NoBrush);
                Painter.setPen(CurrPen);
                Painter.drawPolygon(Polygon);
            }
        }
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

void HexWidget::mousePressEvent(QMouseEvent* event)
{
    if(event->button() == Qt::RightButton)
    {
        IsDragging = true;
        LastMousePos = event->pos();
    }
    else if(event->button() == Qt::LeftButton)
    {
        QPointF Pos = event->pos();

        QPointF Cord = (Pos - QPointF(OffsetX, OffsetY)) / Scale;
        QPoint HexCord = PixelToHex(Cord);

        if(Map.ContainsHex(HexCord.x(), HexCord.y()))
        {
            QPoint HeroCurrPos = Hero.GetPosition();
            const Hex& CurrHex = Map.GetQPointLoc(HeroCurrPos);
            const Hex& TargetHex = Map.GetQPointLoc(HexCord);

            //непробивні
            if(TargetHex.HaveUnit())
            {
                Unit* Unit_ = TargetHex.GetUnit();
                if(Unit_ && Unit_->GetType() == UnitType::StructUnBreak)
                    return;
            }

            if(CurrHex.IsNeighbor(TargetHex))
            {
                QPoint PrevHeroPos = HeroCurrPos;
                Hero.SetPosition(HexCord);

                const Hex& heroIsOnThisHex = Map.GetQPointLoc(Hero.GetPosition());

                if(heroIsOnThisHex.HaveUnit())
                {
                    Unit* unitOnCurrentHex = heroIsOnThisHex.GetUnit();
                    if(unitOnCurrentHex && (unitOnCurrentHex->GetType() == UnitType::Barbarian ||
                                             unitOnCurrentHex->GetType() == UnitType::Warrior ||
                                             unitOnCurrentHex->GetType() == UnitType::Wizard))
                    {
                        qWarning("Hero moved onto an enemy hex! Starting fight.");

                        QPixmap enemyDisplayTexture;

                        if(unitOnCurrentHex->GetType() == UnitType::Barbarian)
                            enemyDisplayTexture = this->BarbarianTexture;
                        else if(unitOnCurrentHex->GetType() == UnitType::Warrior)
                            enemyDisplayTexture = this->WarriorTexture;
                        else if(unitOnCurrentHex->GetType() == UnitType::Wizard)
                            enemyDisplayTexture = this->WizardTexture;

                        if(enemyDisplayTexture.isNull())
                        {
                            qWarning("HexWidget: EnemyTexture is null! Using placeholder for Fight window.");
                            enemyDisplayTexture = QPixmap(200,150);
                            enemyDisplayTexture.fill(Qt::red);
                        }


                        MainHero* heroUnit = &(this->Hero);
                        Unit* enemyUnit = unitOnCurrentHex;


                        Fight* fightDialog = new Fight(enemyDisplayTexture, heroUnit, enemyUnit, this);
                        fightDialog->setAttribute(Qt::WA_DeleteOnClose);

                        int fightResultCode = fightDialog->exec();
                        bool PlayerEscaped = fightDialog->didPlayerEscaped();

                        if (fightResultCode == QDialog::Accepted)
                        {
                            qDebug("Fight won! Enemy removed from hero's current hex.");
                            Map.ClearUnitAt(Hero.GetPosition());
                            Hero.LevelUp();
                            Map.DecrementEnemyCount();
                            if (Map.GetEnemyCount() <= 0) //
                            {
                                QMessageBox::information(this, tr("Victory!"), tr("Congratulations! You have defeated all enemies and won the game!"));
                                emit victory();
                            }
                        }
                        else
                        {
                            if (heroUnit->GetHP() <= 0) { // Якщо HP героя <= 0, то це програш
                                qDebug("Fight lost (Hero HP <= 0). Emitting gameOver signal.");
                                emit gameOver();




                                return;
                            }
                            else if(PlayerEscaped)
                            {
                                // Якщо HP героя > 0, це була втеча
                                qDebug("Hero escaped");
                                Hero.SetPosition(PrevHeroPos);
                            }
                            else
                            {
                                qDebug("Dialog was closed");
                                Hero.SetPosition(PrevHeroPos);

                            }
                        }
                    }

                    else if (unitOnCurrentHex->GetType() == UnitType::Friend)
                    {
                        if (unitOnCurrentHex->GetAI()) {
                            Friendly* friendlyAI = dynamic_cast<Friendly*>(unitOnCurrentHex->GetAI());
                            if (friendlyAI)
                            {
                                std::string greeting = friendlyAI->getGreeting();
                                QMessageBox::information(this, tr("Friendly NPC"), QString::fromStdString(greeting));
                            }
                            else
                            {
                                qWarning("HexWidget: AI for 'Friend' unit is not of Friendly type.");
                            }
                        }
                        else
                        {
                            qWarning("HexWidget: 'Friend' unit has a null AI pointer.");
                        }
                    }

                    //Багаття
                    else if (unitOnCurrentHex->GetType() == UnitType::CampfireUnit)
                    {
                        qDebug("Hero stepped on a campfire.");
                        CampfireUnit* campfireUnit = dynamic_cast<CampfireUnit*>(unitOnCurrentHex);
                        if (campfireUnit && campfireUnit->GetAI())
                        {
                            Campfire* campfireAI = dynamic_cast<Campfire*>(campfireUnit->GetAI());
                            if (campfireAI) {
                                    double oldHP = Hero.GetHP();
                                    double oldMana = Hero.GetMana();
                                    campfireAI->Heal(&Hero);

                                    double newHP = Hero.GetHP();
                                    double newMana = Hero.GetMana();
                                    double currentCampfireHp = campfireUnit->GetHP();
                                    QMessageBox::information(this, tr("Campfire"),
                                                             tr("You rest at the campfire.\nHP: %1 -> %2\nMana: %3 -> %4\nYou can rest %5 times")
                                                                 .arg(oldHP).arg(newHP).arg(oldMana).arg(newMana).arg(currentCampfireHp-1));


                                    currentCampfireHp = campfireUnit->GetHP();
                                    campfireUnit->SetHP(currentCampfireHp - 1);
                                    qDebug() << "[CAMPFIRE_DEBUG] After SetHp(" << currentCampfireHp - 1 << "): ID=" << campfireUnit << "HP=" << campfireUnit->GetHP() << "MaxHP=" << campfireUnit->GetMaxHP();


                                    if (campfireUnit->GetHP() <= 0)
                                    {
                                        Map.ClearUnitAt(Hero.GetPosition());
                                        qDebug("[CAMPFIRE_DEBUG] The campfire has extinguished. HP=%f", campfireUnit->GetHP());
                                        QMessageBox::information(this, tr("Campfire"), tr("The campfire has extinguished."));
                                    } else {
                                        qDebug() << "[CAMPFIRE_DEBUG] Багаття ще горить. HP=" << campfireUnit->GetHP();
                                    }

                            } else {
                                qWarning("HexWidget: Campfire unit's AI is not of Campfire type.");
                            }
                        }
                        else
                        {
                            qWarning("HexWidget: Campfire unit is null or has a null AI pointer.");
                        }
                    }
                    else if(unitOnCurrentHex->GetType() == UnitType::StructBreak)
                    {
                        QMessageBox::information(this, tr("Сундук зі скарбами"),tr("Ти знайшов сундук, рівень підвищено!"));
                        Hero.LevelUp();
                        Map.ClearUnitAt(Hero.GetPosition());
                        qDebug("Hero found treasure");
                    }
                }
                Map.UpdateVisibility(Hero.GetPosition());
                update();
            }
            else
            {
                update();
            }
        }
        else
        {
            update();
        }
        emit heroStatsChanged();
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
