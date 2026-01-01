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

    void parseJsonToVector(const QJsonArray& jsonArr, std::vector<SpellDefinition>& targetList);
    Spell calculateSpellStats(const SpellDefinition& definition, int level);

    SpellType stringToSpellType(const QString& typeStr);

public:
    static SpellManager& getInstance();

    bool loadHeroSpells(const QString& path);
    bool loadEnemySpells(const QString& path);

    std::vector<Spell> getHeroSpellsByIds(const std::vector<std::string>& unlockedIds, int level);
    std::vector<Spell> getEnemySpellsForLevel(int level);
    bool heroSpellExists(const std::string& id);
};

#endif // SPELLMANAGER_H
