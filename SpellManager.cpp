#include "SpellManager.h"
#include <QDebug>
#include <cmath>

SpellManager& SpellManager::Instance()
{
    static SpellManager instance;
    return instance;
}

bool SpellManager::LoadHeroSpells(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open hero spell file:" << path;
        return false;
    }
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    heroSpellDefinitions.clear();
    ParseJsonToVector(doc.array(), heroSpellDefinitions);
    return true;
}

bool SpellManager::LoadEnemySpells(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open enemy spell file:" << path;
        return false;
    }
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    enemySpellDefinitions.clear();
    ParseJsonToVector(doc.array(), enemySpellDefinitions);
    return true;
}

SpellType SpellManager::StringToSpellType(const QString& typeStr) {
    if (typeStr == "fire_spell") return SpellType::Fire;
    if (typeStr == "ice_spell") return SpellType::Ice;
    if (typeStr == "electric_spell") return SpellType::Electric;
    if (typeStr == "arcane_spell") return SpellType::Arcane;
    if (typeStr == "earth_spell") return SpellType::Earth;
    if (typeStr == "wind_spell") return SpellType::Wind;
    if (typeStr == "poison_spell") return SpellType::Poison;
    if (typeStr == "dark_spell") return SpellType::Dark;
    if (typeStr == "holy_spell") return SpellType::Holy;

    return SpellType::Unknown;
}

void SpellManager::ParseJsonToVector(const QJsonArray& jsonArr, std::vector<SpellDefinition>& targetList)
{
    for (const QJsonValue& val : jsonArr) {
        QJsonObject obj = val.toObject();
        SpellDefinition definition;
        definition.id = obj["id"].toString().toStdString();
        definition.name = obj["name"].toString().toStdString();
        definition.type = StringToSpellType(obj["type"].toString());
        definition.baseMana = obj["base_mana"].toDouble();
        definition.baseDamage = obj["base_damage"].toDouble();
        definition.dmgGrowth = obj["dmg_growth"].toDouble();
        definition.manaGrowth = obj["mana_growth"].toDouble();
        definition.requiredLevel = obj["required_level"].toInt();

        targetList.push_back(definition);
    }
}

Spell SpellManager::CalculateSpellStats(const SpellDefinition& definition, int level)
{
    int levelsAboveOne = (level > 1) ? (level - 1) : 0;

    double damageMultiplier = 1.0 + (definition.dmgGrowth * levelsAboveOne);
    double currentDamage = definition.baseDamage * damageMultiplier;

    double manacostMultiplier = 1.0 + (definition.manaGrowth * levelsAboveOne);
    double currentManacost = definition.baseMana * manacostMultiplier;

    if (definition.manaGrowth < 0) {
        double minManacostFromBase = definition.baseMana * 0.30;
        if (currentManacost < minManacostFromBase) {
            currentManacost = minManacostFromBase;
        }
    }
    if (currentManacost < 1.0 && definition.baseMana > 0) currentManacost = 1.0;
    if (definition.baseMana > 0 && currentManacost <= 0) currentManacost = 1.0;
    if (currentDamage < 0) currentDamage = 0;

    return Spell(definition.id, definition.name, definition.type, currentManacost, currentDamage);
}

std::vector<Spell> SpellManager::GetHeroSpellsForLevel(int level)
{
    std::vector<Spell> result;
    for (const auto& definition : heroSpellDefinitions) {
        if (level >= definition.requiredLevel) {
            result.push_back(CalculateSpellStats(definition, level));
        }
    }
    return result;
}

std::vector<Spell> SpellManager::GetEnemySpellsForLevel(int level)
{
    std::vector<Spell> result;
    for (const auto& definition : enemySpellDefinitions) {
        if (level >= definition.requiredLevel) {
            result.push_back(CalculateSpellStats(definition, level));
        }
    }
    return result;
}
