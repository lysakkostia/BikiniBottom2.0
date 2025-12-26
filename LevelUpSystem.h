#ifndef LEVELUPSYSTEM_H
#define LEVELUPSYSTEM_H

#include <QString>
#include <vector>
#include <random>
#include <algorithm>
#include <map>
#include "SpellManager.h"

enum class UpgradeType {
    StatIncrease,
    SpellTypeBuff,
    NewSpellUnlock
};

struct UpgradeOption {
    QString title;
    QString description;
    UpgradeType type;

    double value;
    SpellType specificType;
    int statIndex;

    UpgradeOption()
        : title(""), description(""), type(UpgradeType::StatIncrease),
        value(0), specificType(SpellType::Unknown), statIndex(-1) {}

    UpgradeOption(QString t, QString d, UpgradeType ty, double val, SpellType st = SpellType::Unknown, int stat = -1)
        : title(t), description(d), type(ty), value(val), specificType(st), statIndex(stat) {}
};

class LevelUpGenerator {
private:
    static QString GetSpellTypeName(SpellType type) {
        switch (type) {
        case SpellType::Fire: return "Fire";
        case SpellType::Ice: return "Ice";
        case SpellType::Electric: return "Electric";
        case SpellType::Poison: return "Poison";
        case SpellType::Dark: return "Dark";
        case SpellType::Holy: return "Holy";
        default: return "Unknown";
        }
    }

public:
    static std::vector<UpgradeOption> GenerateOptions() {
        std::vector<UpgradeOption> pool;

        pool.push_back(UpgradeOption("Vitality Boost", "Increases Max HP by 50.", UpgradeType::StatIncrease, 50.0, SpellType::Unknown, 0));
        pool.push_back(UpgradeOption("Arcane Mind", "Increases Max Mana by 30.", UpgradeType::StatIncrease, 30.0, SpellType::Unknown, 1));

        std::vector<SpellType> elements = {
            SpellType::Fire, SpellType::Ice, SpellType::Electric,
            SpellType::Poison, SpellType::Dark, SpellType::Holy
        };

        for (const auto& el : elements) {
            QString name = GetSpellTypeName(el);
            pool.push_back(UpgradeOption(
                name + " Mastery",
                QString("Increases %1 damage by 5%.").arg(name),
                UpgradeType::SpellTypeBuff,
                0.05,
                el
                ));
        }

        std::random_device rd;
        std::mt19937 g(rd());
        std::shuffle(pool.begin(), pool.end(), g);

        if (pool.size() > 3) {
            pool.resize(3);
        }

        return pool;
    }
};

#endif // LEVELUPSYSTEM_H
