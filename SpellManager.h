#ifndef SPELLMANAGER_H
#define SPELLMANAGER_H

#include <QString>
#include <QVector>
#include <QMap>
#include <QFile>
#include <QJsonDocument>
#include <QJsonArray>
#include <QJsonObject>
#include <string>
#include <vector>

enum class SpellType
{
    Fire,
    Ice,
    Electric,
    Poison,
    Dark,
    Holy,
    Unknown
};

struct SpellDefinition
{
    std::string id;
    std::string name;
    SpellType type;
    double baseMana;
    double baseDamage;
    double dmgGrowth;
    double manaGrowth;
    int requiredLevel;
};

struct Spell
{
    std::string id;
    std::string name;
    SpellType type;
    double manacost;
    double damage;

    Spell(std::string _id, std::string _name, SpellType _type, double _mana, double _dmg)
        : id(_id), name(_name), type(_type), manacost(_mana), damage(_dmg) {}
};

class SpellManager
{
private:
    SpellManager() {}
    SpellManager(const SpellManager&) = delete;
    SpellManager& operator=(const SpellManager&) = delete;

    std::vector<SpellDefinition> heroSpellDefinitions;
    std::vector<SpellDefinition> enemySpellDefinitions;

    void ParseJsonToVector(const QJsonArray& jsonArr, std::vector<SpellDefinition>& targetList);
    Spell CalculateSpellStats(const SpellDefinition& definition, int level);

    SpellType StringToSpellType(const QString& typeStr);

public:
    static SpellManager& Instance();

    bool LoadHeroSpells(const QString& path);
    bool LoadEnemySpells(const QString& path);

    std::vector<Spell> GetHeroSpellsByIds(const std::vector<std::string>& unlockedIds, int level);
    std::vector<Spell> GetEnemySpellsForLevel(int level);
    bool HeroSpellExists(const std::string& id);
};

#endif // SPELLMANAGER_H
