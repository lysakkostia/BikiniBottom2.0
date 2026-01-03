#ifndef GAMECONSTANTS_H
#define GAMECONSTANTS_H

namespace GlobalConst
{
    static constexpr float HexSize = 60.0;

    const double GLOBAL_BASE_HP = 200.0;
    const double GLOBAL_BASE_MANA = 100.0;

    namespace Hero {
        const double HP_GROWTH = 10.0; //+ HP за рівень
        const double MANA_GROWTH = 10.0;
    }

    namespace Wizard {
        //(BASE_MULT + level / DIVIDER) * GLOBAL
        const double HP_MULT = 0.7;
        const double HP_DIV = 15.0;
        const double MANA_MULT = 1.5;
        const double MANA_DIV = 5.0;
    }

    namespace Barbarian {
        const double HP_MULT = 1.0;
        const double HP_DIV = 10.0;
        const double MANA_MULT = 1.0;
        const double MANA_DIV = 8.0;
    }

    namespace Warrior {
        const double HP_MULT = 1.3;
        const double HP_DIV = 8.0;
        const double MANA_MULT = 0.8;
        const double MANA_DIV = 12.0;
    }

    namespace Structures {
        const double BREAKABLE_HP = 90.0;
        const double CAMPFIRE_HP = 4.0;
    }

    namespace AI {
        const int INIT_CONFUSED = 30;
        const int INIT_AGGRESSIVE = 50;
        const int INIT_INTELLIGENT = 60;
        const int INIT_HERO = 50;

        const double INTEL_DMG_MULT = 1.10;
        const double INTEL_MANA_MULT = 0.90;
    }

    namespace TextureScale{
        constexpr double HERO = 2.0;
        constexpr double UNIT = 1.7;
        constexpr double MOUNTAIN = 1.9;
        constexpr double STANDART_HEX = 2.0;
    }

    namespace MapGeneration
    {
        namespace Zones {
            constexpr int SAFE_RADIUS = 3;
            constexpr double LEVELS_PER_HEX = 0.75;
            constexpr double DANGER_ZONE_MULT = 1.1;
            constexpr int DANGER_ZONE_FLAT_ADD = 2;
            constexpr int RAND_VAR_MIN = -1;
            constexpr int RAND_VAR_MAX = 2;
        }

        namespace EnemyTypes {
            constexpr double CHANCE_BARBARIAN = 0.35;
            constexpr double CHANCE_WARRIOR = 0.75;
        }

        namespace GarantUnitProbs
        {
            constexpr double CHANCE_FRIEND = 0.025;
            constexpr double CHANCE_CAMPFIRE = 0.02;
            constexpr double CHANCE_CHEST = 0.05;
        }
    }

    namespace NoiseSettings {
        const int DANGER_NOISE_OFFSET = 63472;
        const int TERRAIN_NOISE_OCTAVES_NUM = 3;
        const double TERRAIN_NOISE_PERSISTENCE = 0.5;

        const double SCALE_TERRAIN = 0.17;
        const double SCALE_DANGER = 0.2;
        const double THRESHOLD_MOUNTAIN = 0.60;
        const double THRESHOLD_DANGER_ZONE = 0.52;

        const double CHANCE_ENEMY_IN_ZONE = 0.40;
        const double CHANCE_ENEMY_ROAMING = 0.03;
    }

    namespace FightAI
    {
        constexpr int AI_THINK_DELAY_MS = 1500;
        constexpr int AI_AFTER_PLAYER_DELAY_MS = 1000;
    }

    namespace Progression {
        const double BASE_XP_REQ = 100.0;
        const double XP_GROWTH_FACTOR = 1.2;
        const int SKILL_POINTS_PER_LEVEL = 1;
        const double ENEMY_XP_REWARD = 40.0;
    }
}

#endif // GAMECONSTANTS_H
