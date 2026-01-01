#include "SpellManager.h"
#include <QDebug>
#include <cmath>

SpellManager& SpellManager::getInstance()
{
    static SpellManager instance;
    return instance;
}

bool SpellManager::loadHeroSpells(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open hero spell file:" << path;
        return false;
    }
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    heroSpellDefinitions.clear();
    parseJsonToVector(doc.array(), heroSpellDefinitions);
    return true;
}

bool SpellManager::loadEnemySpells(const QString& path)
{
    QFile file(path);
    if (!file.open(QIODevice::ReadOnly)) {
        qWarning() << "Could not open enemy spell file:" << path;
        return false;
    }
    QByteArray data = file.readAll();
    QJsonDocument doc = QJsonDocument::fromJson(data);
    enemySpellDefinitions.clear();
    parseJsonToVector(doc.array(), enemySpellDefinitions);
    return true;
}

SpellType SpellManager::stringToSpellType(const QString& typeStr) {
    if (typeStr == "fire_spell") return SpellType::Fire;
    if (typeStr == "ice_spell") return SpellType::Ice;
    if (typeStr == "electric_spell") return SpellType::Electric;
    if (typeStr == "poison_spell") return SpellType::Poison;
    if (typeStr == "dark_spell") return SpellType::Dark;
    if (typeStr == "holy_spell") return SpellType::Holy;

    return SpellType::Unknown;
}

void SpellManager::parseJsonToVector(const QJsonArray& jsonArr, std::vector<SpellDefinition>& targetList)
{
    for (const QJsonValue& val : jsonArr) {
        QJsonObject obj = val.toObject();
        SpellDefinition definition;
        definition.id = obj["id"].toString().toStdString();
        definition.name = obj["name"].toString().toStdString();
        definition.type = stringToSpellType(obj["type"].toString());
        definition.baseMana = obj["base_mana"].toDouble();
        definition.baseDamage = obj["base_damage"].toDouble();
        definition.dmgGrowth = obj["dmg_growth"].toDouble();
        definition.manaGrowth = obj["mana_growth"].toDouble();
        definition.requiredLevel = obj["required_level"].toInt();

        targetList.push_back(definition);
    }
}

Spell SpellManager::calculateSpellStats(const SpellDefinition& definition, int level)
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

std::vector<Spell> SpellManager::getHeroSpellsByIds(const std::vector<std::string>& unlockedIds, int level)
{
    std::vector<Spell> result;
    for (const auto& definition : heroSpellDefinitions) {
        for(const std::string& id : unlockedIds) {
            if(definition.id == id) {
                result.push_back(calculateSpellStats(definition, level));
                break;
            }
        }
    }
    return result;
}

bool SpellManager::heroSpellExists(const std::string& id) {
    for (const auto& def : heroSpellDefinitions) {
        if (def.id == id) return true;
    }
    return false;
}

std::vector<Spell> SpellManager::getEnemySpellsForLevel(int level)
{
    std::vector<Spell> result;
    for (const auto& definition : enemySpellDefinitions) {
        if (level >= definition.requiredLevel) {
            result.push_back(calculateSpellStats(definition, level));
        }
    }
    return result;
}
