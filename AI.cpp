#include "AI.h"
#include "Unit.h"
#include "GameConstants.h"
#include "RandomGenerator.h"

AI::AI() : turnOver(0) {}

void AI::initializeSpells(int level, bool isHero, const std::vector<std::string>& heroSpells) {
    spells.clear();
    if (isHero) {
        spells = SpellManager::getInstance().getHeroSpellsByIds(heroSpells, level);
    } else {
        spells = SpellManager::getInstance().getEnemySpellsForLevel(level);
    }
}

const std::vector<Spell>& AI::getSpells() const
{
    return spells;
}

const Spell* AI::chooseBestSpell(double currentMana, double targetHP, double casterHP) const
{
    const Spell* bestSpell = nullptr;
    double maxDamage = -1.0;

    for (const Spell& spell : getSpells()) {
        if (spell.manacost <= currentMana) {
            if (spell.damage > maxDamage) {
                maxDamage = spell.damage;
                bestSpell = &spell;
            }
        }
    }
    return bestSpell;
}

void AI::applyMultipliers(const QMap<SpellType, double>& multipliers)
{
    for (auto &spell : spells) {
        if (multipliers.contains(spell.type)) {
            double mult = multipliers.value(spell.type);
            spell.damage *= mult;
        }
    }
}

void AI::applyManaReductions(const QMap<SpellType, double>& reductions)
{
    for (auto &spell : spells) {
        if (reductions.contains(spell.type)) {
            double reductionPercent = reductions.value(spell.type);
            spell.manacost = spell.manacost * (1.0 - reductionPercent);

            if (spell.manacost < 1.0) spell.manacost = 1.0;
        }
    }
}

Aggresive::Aggresive()
{
    this->turnOver = GlobalConst::AI::INIT_AGGRESSIVE;
}

Confused::Confused()
{
    this->turnOver = GlobalConst::AI::INIT_CONFUSED;
}

const Spell* Confused::chooseBestSpell(double currentMana, double targetHP, double casterHP) const
{
    const std::vector<Spell>& allSpells = this->getSpells();
    std::vector<const Spell*> availableSpellPointers;

    for (const Spell& spell : allSpells) {
        if (spell.manacost <= currentMana) {
            availableSpellPointers.push_back(&spell);
        }
    }

    if (!availableSpellPointers.empty()) {
        int randomIndex = RandGenerator::randIntInInterval(0, availableSpellPointers.size() - 1);
        return availableSpellPointers[randomIndex];
    }

    return nullptr;
}

Intelligent::Intelligent()
{
    this->turnOver = GlobalConst::AI::INIT_INTELLIGENT;
}

const Spell* Intelligent::chooseBestSpell(double currentMana, double targetHP, double casterHP) const
{
    const Spell* bestSpell = nullptr;
    double bestScore = -1000.0;

    bool isDesperate = (casterHP < 40.0);

    for (const Spell& spell : getSpells()) {
        if (spell.manacost > currentMana) continue;

        double score = 0.0;

        if (spell.damage >= targetHP) {
            score += 10000.0;
            score -= spell.manacost;
        }
        else {
            if (isDesperate) {
                score += spell.damage * 2.0;
            } else {
                double dpm = spell.damage / std::max(1.0, spell.manacost);
                score += spell.damage * 0.5 + (dpm * 10.0);
            }
        }

        if (score > bestScore) {
            bestScore = score;
            bestSpell = &spell;
        }
    }

    return bestSpell;
}

void Intelligent::upgradeSpells()
{
    for (auto &spell : spells) {
        spell.damage *= GlobalConst::AI::INTEL_DMG_MULT;
        spell.manacost *= GlobalConst::AI::INTEL_MANA_MULT;
    }
}

MainCharacter::MainCharacter()
{
    this->turnOver = GlobalConst::AI::INIT_HERO;
}

void MainCharacter::updateSpellStats(int playerLevel, const std::vector<std::string>& heroSpells) {
    initializeSpells(playerLevel, true, heroSpells);
}

Friendly::Friendly()
{
    this->turnOver = 0;
}

std::string Friendly::getGreeting() const
{
    return "Greetings, traveler! I am glad to see you in our valley, but be careful, as there is much danger here.";
}

Campfire::Campfire()
{
    this->turnOver = 0;
}

void Campfire::heal(Unit* target) {
    if (target) {
        target->setHP(target->getMaxHP());
        target->setMana(target->getMaxMana());
    }
}
