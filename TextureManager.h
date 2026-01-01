#ifndef TEXTUREMANAGER_H
#define TEXTUREMANAGER_H

#include <QPixmap>
#include <QHash>
#include <QString>
#include "Unit.h"
#include "GameConstants.h"

class TextureManager
{
public:
    TextureManager(const TextureManager&) = delete;
    void operator=(const TextureManager&) = delete;

    static TextureManager& getInstance()
    {
        static TextureManager instance;
        return instance;
    }

    QPixmap getUnitTexture(UnitType type, bool isHeroOnHex = false) const;
    QPixmap getTerrainTexture(bool visible, bool explored) const;
    QPixmap getIcon(const QString& name) const;

private:
    TextureManager();

    void initializeTextures();
    QPixmap loadTexture(const QString &fileName, double scaleFactor);
    QPixmap tintPixmap(const QPixmap& Source, qreal Strength);

    QHash<UnitType, QPixmap> unitTextures;
    QHash<UnitType, QPixmap> heroWithUnitTextures;

    QPixmap fogTexture;
    QPixmap visibleHexTexture;
    QPixmap exploredHexTexture;
};

#endif // TEXTUREMANAGER_H
