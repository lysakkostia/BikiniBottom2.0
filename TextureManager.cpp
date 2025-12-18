#include "TextureManager.h"
#include <QPainter>
#include <QDebug>

namespace Const_Scale = GlobalConst::TextureScale;

TextureManager::TextureManager()
{
    InitializeTextures();
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

void TextureManager::InitializeTextures()
{
    unitTextures[UnitType::MainHero] = loadTexture("NPC5Texture.png", Const_Scale::UNIT);
    unitTextures[UnitType::Barbarian] = loadTexture("NPC4Texture.png", Const_Scale::UNIT);
    unitTextures[UnitType::Warrior] = loadTexture("NPC1Texture.png", Const_Scale::UNIT);
    unitTextures[UnitType::Wizard] = loadTexture("NPC7Texture.png", Const_Scale::UNIT);
    unitTextures[UnitType::Friend] = loadTexture("NPC2Texture.png", Const_Scale::UNIT);
    unitTextures[UnitType::StructBreak] = loadTexture("NPC3Texture.png", Const_Scale::UNIT);
    unitTextures[UnitType::StructUnBreak] = loadTexture("MountainTexture.png", Const_Scale::MOUNTAIN);
    unitTextures[UnitType::CampfireUnit] = loadTexture("NPC6Texture.png", Const_Scale::UNIT);


    heroWithUnitTextures[UnitType::Barbarian] = loadTexture("HeroWithCocosikTexture.png", Const_Scale::UNIT);
    heroWithUnitTextures[UnitType::Warrior] = loadTexture("HeroWithEnemyTexture.png", Const_Scale::UNIT);
    heroWithUnitTextures[UnitType::Wizard] = loadTexture("HeroWithWizardTexture.png", Const_Scale::UNIT);
    heroWithUnitTextures[UnitType::Friend] = loadTexture("HeroWithFriendTexture.png", Const_Scale::UNIT);
    heroWithUnitTextures[UnitType::StructBreak] = loadTexture("HeroWithStructTexture.png", Const_Scale::UNIT);
    heroWithUnitTextures[UnitType::CampfireUnit] = loadTexture("HeroWithCampfireTexture.png", Const_Scale::UNIT);

    fogTexture = loadTexture("FogTexture.png", Const_Scale::STANDART_HEX);
    visibleHexTexture = loadTexture("StandartHex.jpg", Const_Scale::STANDART_HEX);

    if (!visibleHexTexture.isNull()) {
        exploredHexTexture = TintPixmap(visibleHexTexture, 0.4);
    }
}

QPixmap TextureManager::TintPixmap(const QPixmap& Source, qreal Strength)
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
