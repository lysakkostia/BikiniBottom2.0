#include "TextureManager.h"
#include <QPainter>
#include <QDebug>

namespace Const_Scale = GlobalConst::TextureScale;

TextureManager::TextureManager()
{
    initializeTextures();
}

QPixmap TextureManager::loadTexture(const QString &fileName, double scaleFactor)
{
    QPixmap originalPixmap(fileName);

    if (originalPixmap.isNull()) {
        qWarning() << "Failed to load texture:" << fileName;
        return QPixmap();
    }

    return originalPixmap.scaled(
        QSizeF(scaleFactor * GlobalConst::HexSize, scaleFactor * GlobalConst::HexSize).toSize(),
        Qt::KeepAspectRatio, Qt::SmoothTransformation);
}

void TextureManager::initializeTextures()
{
    unitTextures[UnitType::MainHero] = loadTexture(":/textures/MainHero.png", Const_Scale::UNIT);
    unitTextures[UnitType::Barbarian] = loadTexture(":/textures/Barbarian.png", Const_Scale::UNIT);
    unitTextures[UnitType::Warrior] = loadTexture(":/textures/Warrior.png", Const_Scale::UNIT);
    unitTextures[UnitType::Wizard] = loadTexture(":/textures/Wizard.png", Const_Scale::UNIT);
    unitTextures[UnitType::Friend] = loadTexture(":/textures/FriendlyNPC.png", Const_Scale::UNIT);
    unitTextures[UnitType::StructBreak] = loadTexture(":/textures/Treasure.png", Const_Scale::UNIT);
    unitTextures[UnitType::StructUnBreak] = loadTexture(":/textures/Mountain.png", Const_Scale::MOUNTAIN);
    unitTextures[UnitType::CampfireUnit] = loadTexture(":/textures/Campfire.png", Const_Scale::UNIT);

    heroWithUnitTextures[UnitType::Barbarian] = loadTexture(":/textures/HeroWithBarbarianTexture.png", Const_Scale::UNIT);
    heroWithUnitTextures[UnitType::Warrior] = loadTexture(":/textures/HeroWithWarriorTexture.png", Const_Scale::UNIT);
    heroWithUnitTextures[UnitType::Wizard] = loadTexture(":/textures/HeroWithWizardTexture.png", Const_Scale::UNIT);
    heroWithUnitTextures[UnitType::Friend] = loadTexture(":/textures/HeroWithFriendTexture.png", Const_Scale::UNIT);
    heroWithUnitTextures[UnitType::StructBreak] = loadTexture(":/textures/HeroWithTreasureTexture.png", Const_Scale::UNIT);
    heroWithUnitTextures[UnitType::CampfireUnit] = loadTexture(":/textures/HeroWithCampfireTexture.png", Const_Scale::UNIT);

    fogTexture = loadTexture(":/textures/FogTexture.png", Const_Scale::STANDART_HEX);
    visibleHexTexture = loadTexture(":/textures/StandartHex.jpg", Const_Scale::STANDART_HEX);

    if (!visibleHexTexture.isNull()) {
        exploredHexTexture = tintPixmap(visibleHexTexture, 0.4);
    }
}

QPixmap TextureManager::tintPixmap(const QPixmap& Source, qreal Strength)
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

QPixmap TextureManager::getUnitTexture(UnitType type, bool isHeroOnHex) const
{
    if (isHeroOnHex) {
        return heroWithUnitTextures.value(type, unitTextures.value(UnitType::MainHero));
    }
    return unitTextures.value(type);
}

QPixmap TextureManager::getTerrainTexture(bool visible, bool explored) const
{
    if (visible) return visibleHexTexture;
    if (explored) return exploredHexTexture;
    return fogTexture;
}
