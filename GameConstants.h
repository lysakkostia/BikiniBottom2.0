#ifndef GAMECONSTANTS_H
#define GAMECONSTANTS_H

#include <string>
#include <vector>

namespace GlobalConst
{
    const double GLOBAL_BASE_HP = 200.0;
    const double GLOBAL_BASE_MANA = 100.0;

    namespace Hero {
        const double HP_GROWTH = 10.0; //+ HP за рівень
        const double MANA_GROWTH = 10.0;
    }

    namespace Wizard {
        //(BASE_MULT + level / DIVIDER) * GLOBAL
        const double HP_MULT = 0.8;
        const double HP_DIV = 5.0;
        const double MANA_MULT = 1.3;
        const double MANA_DIV = 0.8;
    }

    namespace Barbarian {
        const double HP_MULT = 5.0;
        const double HP_DIV = 4.5;
        const double MANA_MULT = 1.2;
        const double MANA_DIV = 6.0;
    }

    namespace Warrior {
        const double HP_MULT = 2.5;
        const double HP_DIV = 4.0;
        const double MANA_MULT = 1.3;
        const double MANA_DIV = 5.0;
    }

    namespace Structures {
        const double BREAKABLE_HP = 90.0;
        const double CAMPFIRE_HP = 4.0;
    }

    namespace Spells {
        struct SpellData {
            std::string name;
            double manaCost;
            double damage;
            double dmgGrowth;
            double manaGrowth;
        };

        const std::vector<SpellData> BaseSpells = {
            {"Fireball",       10.0, 30.0,  0.08, -0.03},
            {"Ice Blast",      15.0, 48.3,  0.10, -0.04},
            {"Quick Jolt",     4.0,  14.0,  0.07, -0.02},
            {"Frost Shards",   14.0, 50.0,  0.10, -0.04},
            {"Lightning Bolt", 40.0, 70.1,  0.12, -0.05},
            {"Spark",          3.0,  8.0,   0.06, -0.015},
            {"Thunder Strike", 42.0, 85.0,  0.12, -0.05},
            {"Stone Shard",    5.0,  16.0,  0.07, -0.02},
            {"Shockwave",      18.0, 58.0,  0.09, -0.035},
            {"Arcane Missile", 8.0,  25.0,  0.08, -0.025},
            {"TestImba bolt",  2.0,  100000.0, 0.0, 0.0}
        };
    }

    namespace AI {
        const int INIT_CONFUSED = 30;
        const int INIT_AGGRESSIVE = 50;
        const int INIT_INTELLIGENT = 60;
        const int INIT_HERO = 50;

        const double INTEL_DMG_MULT = 1.40;
        const double INTEL_MANA_MULT = 0.70;
    }

    namespace TextureScale{
        constexpr double HERO = 2.0;
        constexpr double UNIT = 1.7;
        constexpr double MOUNTAIN = 1.9;
        constexpr double STANDART_HEX = 2.0;
    }

    namespace MapGeneration
    {
        constexpr double SPAWN_CHANCE = 0.15;

        namespace Zones {
        constexpr double ZONE_1_RATIO = 0.5;
        constexpr double ZONE_2_RATIO = 0.7;
        }

        namespace UnitProbabilities {
        constexpr double THRESHOLD_ENEMY = 0.60;
        constexpr double THRESHOLD_UNBREAK = 0.85;
        constexpr double THRESHOLD_BREAK = 0.92;
        constexpr double THRESHOLD_FRIEND = 0.99;
        //Campfire 1%
        }

        namespace EnemyTypes {
        constexpr double CHANCE_BARBARIAN = 0.4;
        constexpr double CHANCE_WARRIOR = 0.8;
        //Wizard 20%
        }
    }

    namespace FightAI
    {
        constexpr int AI_THINK_DELAY_MS = 1500;
        constexpr int AI_AFTER_PLAYER_DELAY_MS = 1000;
    }
}

#endif // GAMECONSTANTS_H
